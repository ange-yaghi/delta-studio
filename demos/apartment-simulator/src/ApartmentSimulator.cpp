#include "../include/ApartmentSimulator.h"
#include "../include/ApartmentSimulatorWindowHandler.h"

#include "../../../include/yds_tool_geometry_file.h"
#include "../../../include/yds_geometry_preprocessing.h"
#include "../../../include/yds_geometry_export_file.h"

#include "../../../include/yds_timing.h"

#include <fstream>
#include <ctime>

#define FEET(units) ( 12 * ( units ) )
#define INCHES(units) ( units )

float black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
float red[] = { 1.0f, 0.0f, 0.0f, 1.0f };
float yellow[] = { 1.0f, 1.0f, 0.0f, 1.0f };
float brown[] = { 231.0f / 255, 185.0f / 255, 138.0f / 255, 1.0f };
float sky[] = { 138.0f / 255, 219.0f / 255, 255.0f / 255, 1.0f };

Vertex PlaneVertices[] = {
        { { -5.5f, 0.5f, 5.5f },	{ 0.0f, 1.0f } },
        { { 5.5f, 0.5f, 5.5f },		{ 1.0f, 1.0f } },
        { { 5.5f, 0.5f, -5.5f },	{ 1.0f, 0.0f } },
        { { -5.5f, 0.5f, -5.5f },	{ 0.0f, 0.0f } },
};

unsigned short PlaneIndices[6] = {
        0, 1, 2,
        0, 2, 3 };

struct SimpleHeader {
    int NumObjects;
};

ApartmentSimulator::ApartmentSimulator() {
    m_windowSystem = NULL;
    m_inputSystem = NULL;

    m_mainMonitor = NULL;
    m_mainWindow = NULL;

    m_device = NULL;
    m_mainContext = NULL;
    m_mainRenderTarget = NULL;

    m_audioDevice = NULL;

    m_currentAngleX = 0;
    m_currentAngleY = 0.0f;
    m_eyeDir = ysMath::Constants::XAxis;
    m_currentEye = ysMath::LoadVector(0, 2 * 12, 0);
    m_speed = 0;

    m_engineAngle = 0;

    m_shadows = true;
    m_normal = true;
    m_diffuse = true;
    m_chromeMode = false;
    m_tieDye = false;

    m_outside = false;

    m_lightScript.m_manager = &m_sceneManager;

    m_currentSegmentWeight = 0.0;
    m_totalCompletion = 0.0;
}

ApartmentSimulator::~ApartmentSimulator() {
    /* void */
}

void ApartmentSimulator::CompileSceneFile(const char *fname) {
    constexpr int MAX_PATH_LENGTH = 512;

    char fullPath[MAX_PATH_LENGTH];
    strcpy_s(fullPath, MAX_PATH_LENGTH, fname);

    ysToolGeometryFile testFile;
    strcat_s(fullPath, MAX_PATH_LENGTH, ".ysc");
    testFile.Open(GetAssetPath(fullPath).c_str());

    strcpy_s(fullPath, MAX_PATH_LENGTH, fname);
    ysGeometryExportFile exportFile;
    strcat_s(fullPath, MAX_PATH_LENGTH, ".ysce");
    exportFile.Open(GetAssetPath(fullPath).c_str());

    ysObjectData **objects = new ysObjectData * [testFile.GetObjectCount()];
    int objectCount = testFile.GetObjectCount();

    SimpleHeader header;
    header.NumObjects = objectCount;

    exportFile.WriteCustomData(&header, sizeof(SimpleHeader));

    for (int i = 0; i < objectCount; i++) {
        testFile.ReadObject(&objects[i]);
        Material *material = m_sceneManager.FindMaterial(objects[i]->m_materialName);

        if (objects[i]->m_objectInformation.ObjectType == ysObjectData::ObjectType::Geometry) {
            ysGeometryPreprocessing::ResolveSmoothingGroupAmbiguity(objects[i]);
            ysGeometryPreprocessing::CreateAutomaticSmoothingGroups(objects[i]);
            ysGeometryPreprocessing::SeparateBySmoothingGroups(objects[i]);;
            ysGeometryPreprocessing::CalculateNormals(objects[i]);

            bool hasMaterial = material != nullptr;

            if (hasMaterial && material->m_normalMap) {
                ysGeometryPreprocessing::CalculateTangents(objects[i], 0);
            }

            if (hasMaterial && (material->m_normalMap || material->m_specularMap || material->m_diffuseMap)) {
                int startingVerts = objects[i]->m_objectStatistics.NumVertices;

                for (int ii = 0; ii < objects[i]->m_objectStatistics.NumUVChannels; ii++) {
                    ysGeometryPreprocessing::SeparateByUVGroups(objects[i], ii);
                }
            }
        }

        exportFile.WriteObject(objects[i]);
    }

    exportFile.Close();
    testFile.Close();
}

ModelGroup ApartmentSimulator::LoadSceneFile(const char *fname) {
    constexpr int MAX_PATH_LENGTH = 512;
    char fullPath[MAX_PATH_LENGTH];

    strcpy_s(fullPath, MAX_PATH_LENGTH, fname);
    strcat_s(fullPath, MAX_PATH_LENGTH, ".ysce");

    std::fstream file(GetAssetPath(fullPath).c_str(), std::ios::in | std::ios::binary);

    SimpleHeader fileHeader;
    file.read((char *)&fileHeader, sizeof(SimpleHeader));

    int initialIndex = m_sceneManager.GetNextIndex();

    ModelGroup ret;
    ret.s0 = ret.s1 = m_sceneManager.GetObjectCount();

    unsigned short *indicesFile = new unsigned short[2 * 1024 * 1024 / 2]; // 4 MB
    char *verticesFile = (char *)malloc(4 * 1024 * 1024); // 16 MB

    int currentIndexOffset = 0;
    int currentVertexByteOffset = 0;

    ysGPUBuffer *indexBuffer;
    ysGPUBuffer *vertexBuffer;

    m_device->CreateIndexBuffer(&indexBuffer, 1 * 1024 * 1024, NULL, false);
    m_device->CreateVertexBuffer(&vertexBuffer, 4 * 1024 * 1024, NULL, false);

    for (int i = 0; i < fileHeader.NumObjects; i++) {
        ysGeometryExportFile::ObjectOutputHeader header;
        file.read((char *)&header, sizeof(ysGeometryExportFile::ObjectOutputHeader));

        SceneObject *newObject = m_sceneManager.NewSceneObject();
        ret.s1 = max(m_sceneManager.GetObjectCount() - 1, ret.s1);

        ysObjectData::ObjectType type = static_cast<ysObjectData::ObjectType>(header.ObjectType);

        if (type == ysObjectData::ObjectType::Bone
            || type == ysObjectData::ObjectType::Group
            || type == ysObjectData::ObjectType::Plane)
        {
            newObject->m_parent = (header.ParentIndex < 0) ? -1 : header.ParentIndex + initialIndex;
            strcpy_s(newObject->m_name, 64, header.ObjectName);

            newObject->m_material = m_sceneManager.FindMaterial(header.ObjectMaterial);

            // Load Object Transformation
            ysVector translation = ysMath::LoadVector(header.Position);
            ysVector scale = ysMath::LoadVector(header.Scale);

            ysMatrix translationMatrix = ysMath::TranslationTransform(translation);
            ysMatrix scaleMatrix = ysMath::ScaleTransform(scale);

            ysVector x = ysMath::Constants::XAxis;
            ysVector y = ysMath::Constants::YAxis;
            ysVector z = ysMath::Constants::ZAxis;

            ysMatrix rotx = ysMath::RotationTransform(x, header.OrientationEuler.x * ysMath::Constants::PI / (float)180.0);
            ysMatrix roty = ysMath::RotationTransform(y, header.OrientationEuler.y * ysMath::Constants::PI / (float)180.0);
            ysMatrix rotz = ysMath::RotationTransform(z, header.OrientationEuler.z * ysMath::Constants::PI / (float)180.0);

            newObject->ApplyTransformation(translationMatrix);

            newObject->ApplyTransformation(rotz);
            newObject->ApplyTransformation(roty);
            newObject->ApplyTransformation(rotx);

            newObject->ApplyTransformation(scaleMatrix);

            if (type == ysObjectData::ObjectType::Plane) {
                file.read((char *)(&newObject->m_length), sizeof(float));
                file.read((char *)(&newObject->m_width), sizeof(float));
            }
        }
        else if (type == ysObjectData::ObjectType::Geometry) {
            int vertexDataSize = header.VertexDataSize;
            int stride = header.VertexDataSize / header.NumVertices;

            if ((currentVertexByteOffset % stride) != 0)
                currentVertexByteOffset += (stride - (currentVertexByteOffset % stride));

            file.read((char *)(verticesFile + currentVertexByteOffset), vertexDataSize);
            file.read((char *)(indicesFile + currentIndexOffset), sizeof(unsigned short) * header.NumFaces * 3);

            newObject->m_vertexSize = stride;
            newObject->m_numUVChannels = header.NumUVChannels;
            newObject->m_numVertices = header.NumVertices;
            newObject->m_numFaces = header.NumFaces;
            newObject->m_parent = (header.ParentIndex < 0) ? -1 : header.ParentIndex + initialIndex;
            newObject->m_baseIndex = currentIndexOffset;
            newObject->m_baseVertex = currentVertexByteOffset / stride;
            newObject->m_vertexBuffer = vertexBuffer;
            newObject->m_indexBuffer = indexBuffer;
            strcpy_s(newObject->m_name, 64, header.ObjectName);

            newObject->m_material = m_sceneManager.FindMaterial(header.ObjectMaterial);

            currentIndexOffset += header.NumFaces * 3;
            currentVertexByteOffset += header.VertexDataSize;

            if (strcmp(header.ObjectName, "DoorLight") == 0) {
                m_doorLightPosition = header.Position;
            }

            // Load Object Transformation
            ysVector translation = ysMath::LoadVector(header.Position);
            ysVector scale = ysMath::LoadVector(header.Scale);

            ysMatrix translationMatrix = ysMath::TranslationTransform(translation);
            ysMatrix scaleMatrix = ysMath::ScaleTransform(scale);

            ysVector x = ysMath::Constants::XAxis;
            ysVector y = ysMath::Constants::YAxis;
            ysVector z = ysMath::Constants::ZAxis;

            ysMatrix rotx = ysMath::RotationTransform(x, header.OrientationEuler.x * ysMath::Constants::PI / (float)180.0);
            ysMatrix roty = ysMath::RotationTransform(y, header.OrientationEuler.y * ysMath::Constants::PI / (float)180.0);
            ysMatrix rotz = ysMath::RotationTransform(z, header.OrientationEuler.z * ysMath::Constants::PI / (float)180.0);

            newObject->ApplyTransformation(translationMatrix);

            //newObject->ApplyTransformation(rotz);
            //newObject->ApplyTransformation(roty);
            //newObject->ApplyTransformation(rotx);

            newObject->ApplyTransformation(rotz);
            newObject->ApplyTransformation(roty);
            newObject->ApplyTransformation(rotx);

            newObject->ApplyTransformation(scaleMatrix);

            newObject->m_maxPoint = ysMath::LoadVector(header.MaxExtreme);
            newObject->m_minPoint = ysMath::LoadVector(header.MinExtreme);
        }
    }

    m_device->EditBufferData(indexBuffer, (char *)indicesFile);
    m_device->EditBufferData(vertexBuffer, (char *)verticesFile);

    delete[] indicesFile;
    free(verticesFile);

    file.close();

    return ret;
}

void ApartmentSimulator::UpdateShaderConstants() {
    m_device->EditBufferData(m_constantBuffer, (char *)&m_constantData);
}

void ApartmentSimulator::UpdateLighting() {
    m_device->EditBufferData(m_lightBuffer, (char *)&m_lightData);
}

std::string ApartmentSimulator::GetAssetPath(const char *relativePath) const {
    return "../../demos/apartment-simulator/assets/" + std::string(relativePath);
}

void ApartmentSimulator::Initialize(ysDevice::DeviceAPI API, void *instance) {
    // Create Window System
    ysWindowSystem::CreateWindowSystem(&m_windowSystem, ysWindowSystem::Platform::Windows);
    m_windowSystem->ConnectInstance(instance); // Connect the window system to this instance

    // Create Audio System
    ysAudioSystem::CreateAudioSystem(&m_audioSystem, ysAudioSystem::API::DirectSound8);
    m_audioSystem->EnumerateDevices();
    m_audioDevice = m_audioSystem->GetPrimaryDevice();

    // Create Input System
    ysInputSystem::CreateInputSystem(&m_inputSystem, ysWindowSystem::Platform::Windows);
    m_windowSystem->AssignInputSystem(m_inputSystem); // Attach input system to window system
    m_inputSystem->Initialize();

    // Get Main Monitor
    m_windowSystem->SurveyMonitors();
    m_mainMonitor = m_windowSystem->GetMonitor(0);

    // Create Title Window
    m_windowSystem->NewWindow(&m_titleWindow);
    int splashWidth = 512;
    int splashHeight = 384;
    m_titleWindow->AttachEventHandler(NULL);
    m_titleWindow->InitializeWindow(
        NULL,
        "Apartment Simulator | 2014",
        ysWindow::WindowStyle::Popup,
        m_mainMonitor->GetOriginX() + m_mainMonitor->GetWidth() / 2 - splashWidth / 2,
        m_mainMonitor->GetOriginY() + m_mainMonitor->GetHeight() / 2 - splashHeight / 2,
        splashWidth,
        splashHeight,
        m_mainMonitor);

    // Create Main Window
    ApartmentSimulatorWindowHandler *windowHandler = new ApartmentSimulatorWindowHandler;

    m_windowSystem->NewWindow(&m_mainWindow);

    m_mainWindow->AttachEventHandler(NULL); // No handler for now
    m_mainWindow->InitializeWindow(
        NULL,
        "Apartment Simulator | 2014",
        ysWindow::WindowStyle::Windowed,
        m_mainMonitor);

    m_mainWindow->SetState(ysWindow::WindowState::Maximized);
    m_mainWindow->AttachEventHandler(windowHandler);

    m_audioSystem->ConnectDevice(m_audioDevice, m_mainWindow); // Attach sound device to main window

    // Create Graphics Device
    ysDevice::CreateDevice(&m_device, API);
    m_device->InitializeDevice();

    // Create Rendering Context
    m_device->CreateRenderingContext(&m_mainContext, m_mainWindow);
    m_device->CreateRenderingContext(&m_titleContext, m_titleWindow);

    windowHandler->m_device = m_device;
    windowHandler->m_context = m_mainContext;

    // Create Render Target
    m_device->CreateOnScreenRenderTarget(&m_mainRenderTarget, m_mainContext, true);
    m_device->CreateOnScreenRenderTarget(&m_titleRenderTarget, m_titleContext, false);

    m_format.AddChannel("POSITION", 0, ysRenderGeometryChannel::ChannelFormat::R32G32B32A32_FLOAT);
    m_format.AddChannel("TEXCOORD", sizeof(float) * 4, ysRenderGeometryChannel::ChannelFormat::R32G32_FLOAT);
    m_format.AddChannel("NORMAL", sizeof(float) * 6, ysRenderGeometryChannel::ChannelFormat::R32G32B32A32_FLOAT);

    m_simpleFormat.AddChannel("POSITION", 0, ysRenderGeometryChannel::ChannelFormat::R32G32B32_FLOAT);
    m_simpleFormat.AddChannel("TEXCOORD", sizeof(float) * 3, ysRenderGeometryChannel::ChannelFormat::R32G32_FLOAT);

    m_normalMappingFormat.AddChannel("POSITION", 0, ysRenderGeometryChannel::ChannelFormat::R32G32B32A32_FLOAT);
    m_normalMappingFormat.AddChannel("TEXCOORD", sizeof(float) * 4, ysRenderGeometryChannel::ChannelFormat::R32G32_FLOAT);
    m_normalMappingFormat.AddChannel("NORMAL", sizeof(float) * 6, ysRenderGeometryChannel::ChannelFormat::R32G32B32A32_FLOAT);
    m_normalMappingFormat.AddChannel("TANGENT", sizeof(float) * 10, ysRenderGeometryChannel::ChannelFormat::R32G32B32A32_FLOAT);

    m_device->CreateConstantBuffer(&m_constantBuffer, sizeof(ConstantBuffer), NULL, false);
    m_device->CreateConstantBuffer(&m_lightBuffer, sizeof(LightsList), NULL, false);
    m_device->CreateConstantBuffer(&m_objectBuffer, sizeof(ObjectBuffer), NULL, false);
    m_device->CreateConstantBuffer(&m_titleControlsBuffer, sizeof(TitleScreenControls), NULL, false);

    // Shaders

    if (API == ysContextObject::DeviceAPI::DirectX10 || API == ysContextObject::DeviceAPI::DirectX11) {
        m_device->CreateVertexShader(&m_vertexShader, GetAssetPath("shaders/shader_simple.fx").c_str(), "VS");
        m_device->CreatePixelShader(&m_pixelShader, GetAssetPath("shaders/shader_simple.fx").c_str(), "PS");

        m_device->CreateVertexShader(&m_tangentDisplayV, GetAssetPath("shaders/shader_simple.fx").c_str(), "VS_TANGENT");

        // Title Shaders
        m_device->CreateVertexShader(&m_titleVertexShader, GetAssetPath("shaders/title_shader.fx").c_str(), "VS");
        m_device->CreatePixelShader(&m_titlePixelShader, GetAssetPath("shaders/title_shader.fx").c_str(), "PS");
    }
    else if (API == ysContextObject::DeviceAPI::OpenGL4_0) {
        m_device->CreateVertexShader(&m_vertexShader, GetAssetPath("shaders/shader_simple.glvs").c_str(), "VS");
        m_device->CreatePixelShader(&m_pixelShader, GetAssetPath("shaders/shader_simple.glfs").c_str(), "PS");

        m_device->CreateVertexShader(&m_tangentDisplayV, GetAssetPath("shaders/tangent_vertex.glvs").c_str(), "VS_TANGENT");
    }

    m_device->CreateShaderProgram(&m_tangentDisplayProgram);
    m_device->CreateShaderProgram(&m_shaderProgram);
    m_device->CreateShaderProgram(&m_titleShaderProgram);

    // Input Layout
    m_device->CreateInputLayout(&m_inputLayout, m_vertexShader, &m_format);
    m_device->AttachShader(m_shaderProgram, m_vertexShader);
    m_device->AttachShader(m_shaderProgram, m_pixelShader);
    m_device->LinkProgram(m_shaderProgram);

    m_device->CreateInputLayout(&m_normalMappingLayout, m_tangentDisplayV, &m_normalMappingFormat);
    m_device->AttachShader(m_tangentDisplayProgram, m_tangentDisplayV);
    m_device->AttachShader(m_tangentDisplayProgram, m_pixelShader);
    m_device->LinkProgram(m_tangentDisplayProgram);

    m_device->CreateInputLayout(&m_simpleLayout, m_titleVertexShader, &m_simpleFormat);
    m_device->AttachShader(m_titleShaderProgram, m_titleVertexShader);
    m_device->AttachShader(m_titleShaderProgram, m_titlePixelShader);
    m_device->LinkProgram(m_titleShaderProgram);

    // Initialize Scene

    Vertex fullPlane[] = {
        { { -1.0f, 1.0f, 0.5f },	{ 0.0f, 1.0f } },
        { { 1.0f, 1.0f, 0.5f },		{ 1.0f, 1.0f } },
        { { 1.0f, -1.0f, 0.5f },	{ 1.0f, 0.0f } },
        { { -1.0f, -1.0f, 0.5f },	{ 0.0f, 0.0f } },
    };

    unsigned short indices[] = {
        0, 1, 2,
        0, 2, 3 };

    m_device->CreateTexture(&m_titleImage, GetAssetPath("art/SpadinaApartmentCard.png").c_str());
    m_device->CreateVertexBuffer(&m_genericPlane, sizeof(fullPlane), (char *)fullPlane);
    m_device->CreateIndexBuffer(&m_genericPlaneIndices, sizeof(indices), (char *)indices);

    m_mainWindow->SetState(ysWindow::WindowState::Hidden);
    UpdateTitleScreen(0);

    // Constant Buffer
    memcpy(m_constantData.Color, red, sizeof(float) * 4);
    memcpy(m_constantData.Offset, black, sizeof(float) * 4);
    m_constantData.Scale = 1.0;

    m_keyboard = m_inputSystem->GetKeyboardAggregator();
    m_keyboard->AttachDependency();

    CreateSceneView();
    CreateSounds();

    m_currentSegmentWeight = 0.25;
    CreateMaterials();
    m_totalCompletion += m_currentSegmentWeight;

    UpdateTitleScreen(0.0);
    m_currentSegmentWeight = 0.25;
    CompileSceneAssets();
    m_totalCompletion += m_currentSegmentWeight;

    UpdateTitleScreen(0.0);
    m_currentSegmentWeight = 0.5;
    LoadSceneAssets();
    m_totalCompletion += m_currentSegmentWeight;

    m_microwaveClock.Create(&m_sceneManager, 1);
    m_microwaveClock.Display(66, 66);

    m_alarmClock.Create(&m_sceneManager, 2);
    m_alarmClock.m_disableAMPM = false;
    m_alarmClock.m_displayFirstDigit0 = false;
    m_alarmClock.m_restrictedFirstDigit = true;
    m_alarmClock.Display(66, 66);

    m_ovenClock.Create(&m_sceneManager, 3);
    m_ovenClock.Display(66, 66);

    // End of loading
    m_titleWindow->Close();
    m_windowSystem->ProcessMessages();
    m_mainWindow->SetState(ysWindow::WindowState::MAXIMIZED);

    m_device->SetRenderTarget(m_mainRenderTarget);

    m_device->UseShaderProgram(m_shaderProgram);
    m_device->UseInputLayout(m_inputLayout);

    m_device->UseConstantBuffer(m_constantBuffer, 0);
    m_device->UseConstantBuffer(m_lightBuffer, 1);
    m_device->UseConstantBuffer(m_objectBuffer, 2);
}

void ApartmentSimulator::CreateSceneView() {
    m_constantData.View = ysMath::LoadIdentity();
    m_constantData.Projection = ysMath::LoadIdentity();
    m_constantData.World = ysMath::LoadIdentity();
}

void ApartmentSimulator::CreateSounds() {
    m_ambientSoundFile.OpenFile(GetAssetPath("Audio/Ambient/Ambient.wav").c_str());
    m_ambientSoundFile.InitializeInternalBuffer(20000);
    m_ambientSoundBuffer = m_audioDevice->CreateBuffer(m_ambientSoundFile.GetAudioParameters(), 40000);
    m_ambientSoundStream.AttachAudioBuffer(m_ambientSoundBuffer);
    m_ambientSoundStream.AttachAudioFile(&m_ambientSoundFile);
    m_ambientSoundStream.InitializeBuffer();
    m_ambientSoundBuffer->SetMode(ysAudioBuffer::Mode::Stop);

    m_outsideSoundSource.SetBuffer(m_ambientSoundBuffer);
    m_outsideSoundSource.m_volume = 1.00;
    m_outsideSoundSource.m_falloff = true;
    m_outsideSoundSource.m_position = ysMath::LoadVector(0, 4 * 12, -40 * 12);
    m_outsideSoundSource.m_falloffStart = 35 * 12;
    m_outsideSoundSource.m_falloffDistance = 10 * 12;
}

void ApartmentSimulator::UpdateSceneView() {
    ysVector target = ysMath::LoadVector(cos(m_currentAngleY) * cos(m_currentAngleX), sin(m_currentAngleX), sin(m_currentAngleY) * cos(m_currentAngleX));
    target = ysMath::Add(target, m_currentEye);

    ysVector eyevec = ysMath::Normalize(ysMath::Sub(target, m_currentEye));
    m_eyeDir = eyevec;

    ysVector xyPlaneProjection = ysMath::LoadVector(ysMath::GetZ(eyevec), 0.0f, -ysMath::GetX(eyevec), 0.0f);
    ysVector up = ysMath::Cross(eyevec, xyPlaneProjection);

    ysMatrix ytest = ysMath::FrustrumPerspective(ysMath::Constants::PI / 3.0f, m_mainWindow->GetWidth() / (float)m_mainWindow->GetHeight(), 1.0f, 10000.0f);

    m_constantData.View = ysMath::Transpose(ysMath::CameraTarget(m_currentEye, target, up));
    m_constantData.Projection = ysMath::Transpose(ytest);
    m_constantData.World = ysMath::LoadIdentity();
    m_constantData.Eye = ysMath::GetVector4(m_currentEye);

    m_eyeUp = up;
}

void ApartmentSimulator::UpdateTitleScreen(float completion) {
    float totalCompletion = completion * m_currentSegmentWeight + m_totalCompletion;

    m_device->SetRenderTarget(m_titleRenderTarget);

    m_titleControls.BoxBottom = -1;
    m_titleControls.BoxTop = 1;
    m_titleControls.BoxLeft = -1;
    m_titleControls.BoxRight = 1;
    m_titleControls.BoxColor = ysVector4(1.0, 1.0, 1.0, 1.0);
    m_titleControls.UseTexture = 1;
    m_device->EditBufferData(m_titleControlsBuffer, (char *)&m_titleControls);

    m_device->UseTexture(m_titleImage, 0);
    m_device->UseVertexBuffer(m_genericPlane, sizeof(Vertex), 0);
    m_device->UseIndexBuffer(m_genericPlaneIndices, 0);
    m_device->UseInputLayout(m_simpleLayout);
    m_device->UseShaderProgram(m_titleShaderProgram);
    m_device->UseConstantBuffer(m_titleControlsBuffer, 0);

    m_device->Draw(2, 0, 0);

    m_titleControls.BoxBottom = -0.50f;
    m_titleControls.BoxTop = -0.45f;
    m_titleControls.BoxLeft = -1.0f;
    m_titleControls.BoxRight = -1.0f + totalCompletion * 2;
    m_titleControls.BoxColor = ysVector4(151.0f / 255, 15.0f / 255, 0.0f, 1.0f);
    m_titleControls.UseTexture = 0;
    m_device->EditBufferData(m_titleControlsBuffer, (char *)&m_titleControls);

    m_device->Draw(2, 0, 0);
    m_device->Present();

    m_windowSystem->ProcessMessages();
}

void ApartmentSimulator::GameLoop() {
    //m_mainWindow->SetWindowStyle(ysWindow::WINDOW_STYLE_FULLSCREEN);
    //m_device->SetContextMode(m_mainContext, ysRenderingContext::CONTEXT_MODE_FULLSCREEN);

    GenerateOcclusionGraph();

    InitializeLighting();
    UpdateLighting();

    float prev = m_lightData.Lights[0].Intensity;
    float prevDoor = m_lightData.Lights[4].Intensity;

    for (int i = 0; i < m_sceneManager.GetObjectCount(); i++) {
        m_sceneManager.GetObjectA(i)->UpdateExtents();
    }

    std::ofstream framerateFile("framerate.txt");

    m_lightScript.Execute(GetAssetPath("light/DeskLighting.ls").c_str());
    m_lightScript.Execute(GetAssetPath("light/Lights_1.ls").c_str());
    m_lightScript.Execute(GetAssetPath("light/ShadowScript.ls").c_str());

    UpdateSceneView();
    m_outsideSoundSource.Update(m_currentEye, m_eyeDir, m_eyeUp);
    m_ambientSoundBuffer->SetMode(ysAudioBuffer::Mode::Loop);

    // Main Game Loop
    while (m_mainWindow->IsOpen()) {
        unsigned __int64 frameStart = ysTimingSystem::Get()->GetClock();

        m_device->ClearBuffers(black);

        float timePassed = (float)ysTimingSystem::Get()->GetFrameDuration();

        ysKeyboard *keyboard = m_keyboard->GetAsKeyboard();
        if (keyboard->GetKey(ysKeyboard::KEY_A)->IsDown()) {
            m_speed += 100 * timePassed;
        }
        else if (keyboard->GetKey(ysKeyboard::KEY_S)->IsDown()) {
            m_speed -= 100 * timePassed;
        }

        if (keyboard->ProcessKeyTransition(ysKeyboard::KEY_D)) {
            m_speed = 0;
        }

        float angularSpeed = ysMath::Constants::PI / 1.5f;

        if (keyboard->GetKey(ysKeyboard::KEY_RIGHT)->IsDown()) {
            m_angularSpeedY = -angularSpeed;
        }
        else if (keyboard->GetKey(ysKeyboard::KEY_LEFT)->IsDown()) {
            m_angularSpeedY = angularSpeed;
        }
        else m_angularSpeedY = 0.0f;

        if (keyboard->GetKey(ysKeyboard::KEY_UP)->IsDown()) {
            m_angularSpeedX = -angularSpeed;
        }
        else if (keyboard->GetKey(ysKeyboard::KEY_DOWN)->IsDown()) {
            m_angularSpeedX = angularSpeed;
        }
        else m_angularSpeedX = 0.0f;

        if (keyboard->ProcessKeyTransition(ysKeyboard::KEY_L)) {
            m_kitchenLight->m_on = !m_kitchenLight->m_on;
            m_sceneManager.FindObject("KitchenLightBall")->m_material = (m_kitchenLight->m_on) ? m_lightBulb : m_fridgeWhite;
        }

        if (keyboard->ProcessKeyTransition(ysKeyboard::KEY_K)) {
            m_doorLight->m_on = !m_doorLight->m_on;
            m_sceneManager.FindObject("DoorLight")->m_material = (m_doorLight->m_on) ? m_lightBulb : m_fridgeWhite;
        }

        if (keyboard->ProcessKeyTransition(ysKeyboard::KEY_J)) {
            LightObject *light = m_sceneManager.FindLight("HallwayLight");

            light->m_on = !light->m_on;

            m_sceneManager.FindObject("HallwayLightBulb")->m_material = (light->m_on) ? m_lightBulb : m_fridgeWhite;
            m_sceneManager.FindObject("HallwayLightCasing")->m_material = (light->m_on) ? m_lightBulbCasing : m_fridgeWhite;
        }

        if (keyboard->ProcessKeyTransition(ysKeyboard::KEY_B)) {
            for (int i = 0; i < 6; i++) {
                m_bathroomLights[i]->m_on = !m_bathroomLights[i]->m_on;
            }

            m_sceneManager.FindObject("Lightbulb0")->m_material = (m_bathroomLights[0]->m_on) ? m_lightBulb : m_fridgeWhite;
            m_sceneManager.FindObject("Lightbulb1")->m_material = (m_bathroomLights[0]->m_on) ? m_lightBulb : m_fridgeWhite;
            m_sceneManager.FindObject("Lightbulb2")->m_material = (m_bathroomLights[0]->m_on) ? m_lightBulb : m_fridgeWhite;
            m_sceneManager.FindObject("Lightbulb3")->m_material = (m_bathroomLights[0]->m_on) ? m_lightBulb : m_fridgeWhite;
            m_sceneManager.FindObject("Lightbulb4")->m_material = (m_bathroomLights[0]->m_on) ? m_lightBulb : m_fridgeWhite;
        }

        if (keyboard->ProcessKeyTransition(ysKeyboard::KEY_O)) {
            for (int i = 0; i < 2; i++) {
                m_lampLights[i]->m_on = !m_lampLights[i]->m_on;
            }

            m_lampMaterial->m_lit = (m_lampLights[0]->m_on) ? 0 : 1;

            m_sceneManager.FindObject("LargeLampBulb")->m_material = (m_lampLights[0]->m_on) ? m_lightBulb : m_fridgeWhite;
        }

        if (keyboard->ProcessKeyTransition(ysKeyboard::KEY_R)) {
            m_roomAmbient->m_on = !m_roomAmbient->m_on;
        }

        if (keyboard->ProcessKeyTransition(ysKeyboard::KEY_E)) {
            for (int i = 0; i < 2; i++) {
                m_exteriorLights[i]->m_on = !m_exteriorLights[i]->m_on;
            }
        }

        if (keyboard->ProcessKeyTransition(ysKeyboard::KEY_F)) {
            m_lightScript.Execute(GetAssetPath("light/DeskLighting.ls").c_str());
            m_lightScript.Execute(GetAssetPath("light/ShadowScript.ls").c_str());
        }

        if (keyboard->ProcessKeyTransition(ysKeyboard::KEY_C)) {
            m_sceneManager.FindLight("ComputerScreen")->m_on = !m_sceneManager.FindLight("ComputerScreen")->m_on;
            if (m_sceneManager.FindLight("ComputerScreen")->m_on) {
                m_sceneManager.FindObject("Monitor1Screen")->m_material = m_sceneManager.FindMaterial("Monitor1");
            }
            else {
                m_sceneManager.FindObject("Monitor1Screen")->m_material = m_sceneManager.FindMaterial("BlackPaint");
            }
        }

        if (keyboard->GetKey(ysKeyboard::KEY_H)->IsDown()) {
            m_currentEye = ysMath::Add(ysMath::Mask(m_currentEye, ysMath::Constants::MaskOffY), ysMath::LoadVector(0.0f, 5 * 12 + 3, 0.0f));
        }

        if (keyboard->ProcessKeyTransition(ysKeyboard::KEY_1)) {
            m_skyMaterial->m_diffuseMap = m_sky1;
            m_lightScript.Execute(GetAssetPath("light/Lights_1.ls").c_str());
        }

        if (keyboard->ProcessKeyTransition(ysKeyboard::KEY_2)) {
            m_skyMaterial->m_diffuseMap = m_sky2;
            m_lightScript.Execute(GetAssetPath("light/Lights_2.ls").c_str());
        }

        if (keyboard->ProcessKeyTransition(ysKeyboard::KEY_3)) {
            m_skyMaterial->m_diffuseMap = m_sky3;
            m_lightScript.Execute(GetAssetPath("light/Lights_3.ls").c_str());
        }

        if (keyboard->ProcessKeyTransition(ysKeyboard::KEY_4)) {
            m_skyMaterial->m_diffuseMap = m_sky4;
            m_lightScript.Execute(GetAssetPath("light/Lights_4.ls").c_str());
        }

        if (keyboard->ProcessKeyTransition(ysKeyboard::KEY_F1)) {
            m_shadows = !m_shadows;
        }
        else if (keyboard->ProcessKeyTransition(ysKeyboard::KEY_F2)) {
            m_normal = !m_normal;
        }
        else if (keyboard->ProcessKeyTransition(ysKeyboard::KEY_F3)) {
            m_diffuse = !m_diffuse;
        }
        else if (keyboard->ProcessKeyTransition(ysKeyboard::KEY_F4)) {
            m_chromeMode = !m_chromeMode;
        }
        else if (keyboard->ProcessKeyTransition(ysKeyboard::KEY_F5)) {
            m_tieDye = !m_tieDye;
        }
        else if (keyboard->ProcessKeyTransition(ysKeyboard::KEY_ESCAPE)) {
            return;
        }

        UpdateSceneView();

        m_currentAngleX += m_angularSpeedX * timePassed;
        m_currentAngleY += m_angularSpeedY * timePassed;

        ysVector timePassedV = ysMath::LoadScalar((float)ysTimingSystem::Get()->GetFrameDuration());
        ysVector speed = ysMath::LoadScalar(m_speed);
        ysVector delta = ysMath::Mul(speed, m_eyeDir);
        delta = ysMath::Mul(delta, timePassedV);
        m_currentEye = ysMath::Add(m_currentEye, delta);

        memcpy(m_constantData.Color, brown, sizeof(float) * 4);
        m_constantData.Offset[2] = 0.0;
        m_constantData.Offset[1] = 0.0;
        m_constantData.Offset[0] = 0.0;
        m_constantData.AllWhiteLight = 0;
        UpdateShaderConstants();

        m_device->UseShaderProgram(m_shaderProgram);

        UpdateLightExtents();

        // Update Time on Clocks
        time_t t = time(NULL);
        struct tm timeInfo;
        localtime_s(&timeInfo, &t);
        int hour = timeInfo.tm_hour % 12;
        hour = (hour == 0) ? 12 : hour;

        m_microwaveClock.Display(hour, timeInfo.tm_min);
        m_alarmClock.Display(hour, timeInfo.tm_min, timeInfo.tm_hour >= 12);
        m_ovenClock.Display(hour, timeInfo.tm_min);

        GenerateCameraGraph();

        m_totalDrawTime = 0;
        m_totalProcessingTime = 0;
        m_updateTime = 0;
        RenderObjects(m_apartmentGroup, 1);
        RenderObjects(m_bathroomGroup, 1);
        RenderObjects(m_officeGroup, 1);
        RenderObjects(m_fridgeGroup, 1);
        RenderObjects(m_balconyGroup, 1);
        RenderObjects(m_kitchenCabinetsGroup, 1);
        RenderObjects(m_largeClosetGroup, 2);
        RenderObjects(m_doorFramesGroup, 1);
        RenderObjects(m_radiatorGroup, 1);
        RenderObjects(m_kitchenCounterGroup, 1);
        RenderObjects(m_kitchenSinkGroup, 1);
        RenderObjects(m_fixturesGroup, 1);
        RenderObjects(m_bedGroup, 1);
        RenderObjects(m_baseboardGroup, 1);
        RenderObjects(m_ovenGroup, 1);
        RenderObjects(m_environmentGroup, 0);
        RenderObjects(m_clockGroup, 0);

        //RenderObjects(m_volumes, 1);
        //m_device->UseShaderProgram(m_tangentDisplayProgram);
        //m_device->UseInputLayout(m_normalMappingLayout);
        //RenderObjects(m_tangentTestGroup, 1);

        if (m_outside) {
            m_outsideSoundSource.m_volume = 1.4f;
        }
        else {
            m_outsideSoundSource.m_volume = 0.5f;
        }

        m_device->Present();

        m_windowSystem->ProcessMessages();

        m_audioDevice->ProcessAudioBuffers();
        m_ambientSoundStream.Update();

        ysTimingSystem::Get()->Update();

        unsigned __int64 frameLength = ysTimingSystem::Get()->GetClock();
        framerateFile << m_totalDrawTime << "\t" << m_totalProcessingTime << "\t" << m_updateTime << "\t" << frameLength << "\n";

        m_outsideSoundSource.Update(m_currentEye, m_eyeDir, m_eyeUp);
    }
}

void ApartmentSimulator::UseMaterial(Material *material) {
    if (material == NULL && m_chromeMode) material = m_chrome;
    else if (m_tieDye) {
        m_tieDyeMaterial->m_normalMap = material ? material->m_normalMap : NULL;
        material = m_tieDyeMaterial;
    }

    if (material != NULL) {
        m_objectData.Lit = material->m_lit;

        if (m_chromeMode) m_objectData.DiffuseColor = m_chrome->m_diffuseColor;
        else if (m_diffuse) m_objectData.DiffuseColor = material->m_diffuseColor;
        else m_objectData.DiffuseColor = ysVector4(1.0, 1.0, 1.0, 1.0);

        m_objectData.Specular = material->m_specular;
        m_objectData.SpecularPower = material->m_specularPower;
        m_objectData.UseFalloff = material->m_useFalloff;
        m_objectData.FalloffColor = material->m_falloffColor;
        m_objectData.FalloffPower = material->m_falloffPower;
        m_objectData.Ambient = material->m_ambient;
        m_objectData.UseAlpha = material->m_useAlpha;

        if (material->m_diffuseMap != NULL && m_diffuse && !m_chromeMode) {
            m_objectData.UseDiffuseMap = 1;
            m_device->UseTexture(material->m_diffuseMap, 0);
        }
        else m_objectData.UseDiffuseMap = 0;

        if (m_chromeMode) {
            m_objectData.UseReflectMap = 1;
            m_objectData.ReflectPower = m_chrome->m_reflectPower;
            m_device->UseTexture(m_chrome->m_reflectMap, 3);
        }
        else if (material->m_reflectMap != NULL && m_diffuse) {
            m_objectData.UseReflectMap = 1;
            m_objectData.ReflectPower = material->m_reflectPower;
            m_device->UseTexture(material->m_reflectMap, 3);
        }
        else m_objectData.UseReflectMap = 0;

        if (material->m_normalMap != NULL && m_normal) {
            m_objectData.UseNormalMap = 1;
            m_device->UseTexture(material->m_normalMap, 2);
        }
        else m_objectData.UseNormalMap = 0;

        if (material->m_specularMap != NULL) {
            m_objectData.UseSpecularMap = 1;
            m_device->UseTexture(material->m_specularMap, 1);
        }
        else m_objectData.UseSpecularMap = 0;
    }
    else {
        // Load Default
        m_objectData.Lit = 1;
        m_objectData.DiffuseColor = ysVector4(1.0, 1.0, 1.0, 1.0);
        m_objectData.Specular = 0.25;
        m_objectData.SpecularPower = 1.0;
        m_objectData.UseDiffuseMap = 0;
        m_objectData.UseSpecularMap = 0;
        m_objectData.UseNormalMap = 0;
        m_objectData.UseFalloff = 0;
        m_objectData.FalloffColor = ysVector4(1.0, 1.0, 1.0, 1.0);
        m_objectData.FalloffPower = 1.0f;
        m_objectData.Ambient = 1.0f;
        m_objectData.UseAlpha = 0;
        m_objectData.UseReflectMap = 0;
    }
}

void ApartmentSimulator::UpdateLightExtents() {
    for (int i = 0; i < m_sceneManager.m_lightVolumes.GetNumObjects(); i++) {
        for (int b = 0; b < m_sceneManager.m_lightVolumes.Get(i)->m_volumes.GetNumObjects(); b++) {
            m_sceneManager.m_lightVolumes.Get(i)->m_volumes[b]->UpdateExtents();
        }
    }
}

void ApartmentSimulator::RenderObject(SceneObject *object, int lit) {
    unsigned __int64 processingStart = ysTimingSystem::Get()->GetClock();
    if (object->m_hidden) return;

    // Check occlusion graph
    for (int i = 0; i < m_cameraGraph.GetNumObjects(); i++) {
        for (int j = 0; j < m_cameraGraph[i]->m_exclusions.GetNumObjects(); j++) {
            if (object->Inside(m_cameraGraph[i]->m_exclusions[j])) {
                m_totalProcessingTime += (ysTimingSystem::Get()->GetClock() - processingStart);
                return;
            }
        }
    }

    if (!object->m_validLightVolumeCache) {
        object->m_lightVolumeCache.Clear();

        // Check Lights
        int numLightVolumes = m_sceneManager.NumLightVolumes();
        for (int i = 0; i < numLightVolumes; i++) {
            LightVolume *volume = m_sceneManager.m_lightVolumes.Get(i);
            int numObjects = volume->m_volumes.GetNumObjects();

            if (numObjects > 0) volume->m_intermediate = 0;
            else volume->m_intermediate = 1;
            for (int b = 0; b < numObjects; b++) {
                if (volume->m_volumes[b]->Intersecting(object)) {
                    volume->m_intermediate = 1;
                    object->m_lightVolumeCache.New() = volume;
                    break;
                }
            }
        }

        object->m_validLightVolumeCache = true;
    }
    else {
        int numLightVolumes = m_sceneManager.NumLightVolumes();
        for (int i = 0; i < numLightVolumes; i++) {
            LightVolume *volume = m_sceneManager.m_lightVolumes.Get(i);
            volume->m_intermediate = 0;
        }

        for (int i = 0; i < object->m_lightVolumeCache.GetNumObjects(); i++) {
            object->m_lightVolumeCache[i]->m_intermediate = 1;
        }
    }

    // Check Behind Camera
    if (object->Behind(m_currentEye, m_eyeDir)) {
        m_totalProcessingTime += (ysTimingSystem::Get()->GetClock() - processingStart);
        return;
    }

    int lightIndex = 0;
    int numLights = m_sceneManager.NumLights();
    for (int i = 0; i < numLights; i++) {
        LightObject *light = m_sceneManager.m_lights.Get(i);
        if ((light->m_volume == NULL || light->m_volume->m_intermediate == 1) && light->m_on && !light->m_virtual) {
            if (light->m_reference) {
                light->m_reference->m_location = light->m_location;
                SubmitLight(light->m_reference, lightIndex++);
            }
            else {
                SubmitLight(light, lightIndex++);
            }
        }
    }

    if (m_shadows) {
        int shadowIndex = 0;
        int numShadows = m_sceneManager.NumShadows();
        for (int i = 0; i < numShadows; i++) {
            ShadowObject *shadow = m_sceneManager.m_shadows.Get(i);
            if ((shadow->m_volume == NULL || shadow->m_volume->m_intermediate == 1)) {
                SubmitShadow(shadow, shadowIndex++);
            }
        }

        m_lightData.NumShadows = shadowIndex;
    }
    else m_lightData.NumShadows = 0;

    m_lightData.NumLights = lightIndex;

    // End part 1
    m_totalProcessingTime += (ysTimingSystem::Get()->GetClock() - processingStart);
    processingStart = ysTimingSystem::Get()->GetClock();

    UpdateLighting();

    if (object->m_vertexBuffer && object->m_indexBuffer) {
        m_device->UseVertexBuffer(object->m_vertexBuffer, object->m_vertexSize, 0);
        m_device->UseIndexBuffer(object->m_indexBuffer, 0);
    }
    else return;

    if (object->m_material && object->m_material->m_normalMap != NULL) {
        m_device->UseShaderProgram(m_tangentDisplayProgram);
        m_device->UseInputLayout(m_normalMappingLayout);
    }
    else {
        m_device->UseShaderProgram(m_shaderProgram);
        m_device->UseInputLayout(m_inputLayout);
    }

    m_objectData.ObjectTransform = object->GetFinalTransform();
    UseMaterial(object->m_material);

    m_device->EditBufferData(m_objectBuffer, (char *)&m_objectData);

    m_updateTime = (ysTimingSystem::Get()->GetClock() - processingStart);

    // Part 2
    unsigned __int64 drawStart = ysTimingSystem::Get()->GetClock();
    m_device->Draw(object->m_numFaces, object->m_baseIndex, object->m_baseVertex);
    m_totalDrawTime += ysTimingSystem::Get()->GetClock() - drawStart;
}

void ApartmentSimulator::RenderObjects(ModelGroup &group, int lit) {
    for (int i = group.s0; i <= group.s1; i++) {
        SceneObject *object = m_sceneManager.GetObjectA(i);
        RenderObject(object, lit);
    }
}

void ApartmentSimulator::Destroy() {
    m_device->DestroyShader(m_vertexShader);
    m_device->DestroyShader(m_pixelShader);
    m_device->DestroyShaderProgram(m_shaderProgram, false);

    m_device->DestroyGPUBuffer(m_constantBuffer);
    m_device->DestroyGPUBuffer(m_lightBuffer);
    m_device->DestroyGPUBuffer(m_objectBuffer);

    m_device->DestroyInputLayout(m_inputLayout);

    m_device->DestroyRenderTarget(m_mainRenderTarget);
    m_device->DestroyRenderingContext(m_mainContext);

    m_device->DestroyDevice();

    m_windowSystem->DeleteAllWindows();
}
