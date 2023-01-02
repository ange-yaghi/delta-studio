#ifndef APTSIM_APARTMENT_SIMULATOR_H
#define APTSIM_APARTMENT_SIMULATOR_H

#include "../../../include/yds_core.h"
#include "../../../include/yds_device.h"
#include "../../../include/yds_window_system.h"
#include "../../../include/yds_input_system.h"

#include "../../../include/yds_audio_system.h"
#include "../../../include/yds_audio_device.h"
#include "../../../include/yds_audio_buffer.h"

#include "../../../include/yds_audio_wave_file.h"
#include "../../../include/yds_audio_streaming.h"

#include "../../../include/yds_math.h"

#include "Lighting.h"
#include "SceneManager.h"

#include "LCD_Clock.h"
#include "SoundSource.h"

#include "LightScript.h"

#include <string>

struct Vertex {
    float Position[3];
    float Tex[2];
};

struct ConstantBuffer {
    ysMatrix World;
    ysMatrix View;
    ysMatrix Projection;

    ysVector4 Eye;

    float Color[4];
    float Offset[4];

    float Scale;
    int AllWhiteLight;

    int Pad[2];
};

struct ModelGroup {
    int s0, s1;
};

struct ObjectBuffer {
    ysMatrix ObjectTransform;

    ysVector4 DiffuseColor;

    ysVector4 FalloffColor;
    float FalloffPower;

    int UseDiffuseMap;
    int UseSpecularMap;
    int UseNormalMap;
    int UseFalloff;
    int UseAlpha;
    int UseReflectMap;

    float Specular;
    float SpecularPower;
    float ReflectPower;

    float Ambient;

    int Lit;
    int LightExclusion[NUM_LIGHTS];

    int Pad[1];
};

struct TitleScreenControls {
    float BoxLeft;
    float BoxRight;
    float BoxTop;
    float BoxBottom;
    ysVector4 BoxColor;
    int UseTexture;

    int Pad[3];
};

class ApartmentSimulator {
public:
    ApartmentSimulator();
    ~ApartmentSimulator();

    void Initialize(ysDevice::DeviceAPI API, void *instance);
    void GameLoop();
    void Destroy();

    void CreateSceneView();
    void CreateSounds();

    void UpdateSceneView();

    void InitializeLighting();
    void UpdateLighting();

    void CompileSceneFile(const char *fname);
    void CreateMaterials();
    void CompileSceneAssets();
    void LoadSceneAssets();

    ModelGroup LoadSceneFile(const char *fname);

    void UseMaterial(Material *material);
    void RenderObjects(ModelGroup &group, int lit);
    void RenderObject(SceneObject *object, int lit);

    void UpdateTitleScreen(float completion);

    void GenerateOcclusionGraph();

    std::string GetAssetPath(const char *relativePath) const;

protected:
    void UpdateShaderConstants();

    ysWindowSystem *m_windowSystem;
    ysInputSystem *m_inputSystem;
    ysAudioSystem *m_audioSystem;

    ysMonitor *m_mainMonitor;
    ysWindow *m_mainWindow;
    ysWindow *m_titleWindow;

    ysDevice *m_device;
    ysRenderingContext *m_mainContext;
    ysRenderTarget *m_mainRenderTarget;

    ysRenderingContext *m_titleContext;
    ysRenderTarget *m_titleRenderTarget;

    // Geometry
    ysRenderGeometryFormat m_format;
    ysInputLayout *m_inputLayout;

    ysRenderGeometryFormat m_normalMappingFormat;
    ysInputLayout *m_normalMappingLayout;

    ysRenderGeometryFormat m_simpleFormat;
    ysInputLayout *m_simpleLayout;

    TitleScreenControls m_titleControls;
    ysGPUBuffer *m_titleControlsBuffer;

    ysGPUBuffer *m_constantBuffer;
    ysGPUBuffer *m_lightBuffer;
    ysGPUBuffer *m_objectBuffer;

    ysGPUBuffer *m_genericPlane;
    ysGPUBuffer *m_genericPlaneIndices;

    // Environment
    ysTexture *m_sky1;
    ysTexture *m_sky2;
    ysTexture *m_sky3;
    ysTexture *m_sky4;
    ysTexture *m_titleImage;
    Material *m_skyMaterial;
    Material *m_lampMaterial;
    Material *m_fridgeWhite;
    Material *m_lightBulb;
    Material *m_lightBulbCasing;
    Material *m_chrome;
    Material *m_tieDyeMaterial;

    ysTexture *m_noise;

    LightObject *m_roomAmbient;
    LightObject *m_exteriorLights[3];
    LightObject *m_bathroomLights[6];
    LightObject *m_kitchenLight;
    LightObject *m_doorLight;
    LightObject *m_lampLights[2];

    // Shaders
    ysShader *m_vertexShader;
    ysShader *m_pixelShader;
    ysShaderProgram *m_shaderProgram;

    ysShader *m_tangentDisplayV;
    ysShaderProgram *m_tangentDisplayProgram;

    ysShader *m_titleVertexShader;
    ysShader *m_titlePixelShader;
    ysShaderProgram *m_titleShaderProgram;

    // Shader Parameters
    ConstantBuffer m_constantData;
    ObjectBuffer m_objectData;
    LightsList m_lightData;

    // User Input
    ysInputDevice *m_keyboard;

    // Audio
    ysAudioDevice *m_audioDevice;

    SoundSource m_outsideSoundSource;
    ysAudioWaveFile m_ambientSoundFile;
    ysAudioBuffer *m_ambientSoundBuffer;
    ysStreamingAudio m_ambientSoundStream;

    // DERIVED
    SceneManager m_sceneManager;
    LightScript m_lightScript;
    ysExpandingArray<OcclusionVolume *, 4> m_cameraGraph;

    void GenerateCameraGraph();

    void SubmitLight(LightObject *light, int index);
    void SubmitShadow(ShadowObject *shadow, int index);

    bool m_shadows;
    bool m_normal;
    bool m_diffuse;
    bool m_chromeMode;
    bool m_tieDye;

    bool m_outside;

    unsigned __int64 m_totalDrawTime;
    unsigned __int64 m_totalProcessingTime;
    unsigned __int64 m_updateTime;

    float m_currentSegmentWeight;
    float m_totalCompletion;

protected:
    void UpdateLightExtents();

    ModelGroup m_apartmentGroup;
    ModelGroup m_bathroomGroup;
    ModelGroup m_largeClosetGroup;
    ModelGroup m_fridgeGroup;
    ModelGroup m_environmentGroup;
    ModelGroup m_balconyGroup;
    ModelGroup m_kitchenCabinetsGroup;
    ModelGroup m_radiatorGroup;
    ModelGroup m_doorFramesGroup;
    ModelGroup m_ovenGroup;
    ModelGroup m_engineGroup;
    ModelGroup m_kitchenCounterGroup;
    ModelGroup m_kitchenSinkGroup;
    ModelGroup m_fixturesGroup;
    ModelGroup m_bedGroup;
    ModelGroup m_baseboardGroup;
    ModelGroup m_testGroup;
    ModelGroup m_tangentTestGroup;
    ModelGroup m_volumes;
    ModelGroup m_occlusionVolumes;
    ModelGroup m_officeGroup;
    ModelGroup m_clockGroup;

    LCD_Clock m_microwaveClock;
    LCD_Clock m_alarmClock;
    LCD_Clock m_ovenClock;

    float m_currentAngleX;
    float m_currentAngleY;
    float m_speed;
    float m_angularSpeedX;
    float m_angularSpeedY;

    float m_engineAngle;

    ysVector m_currentEye;
    ysVector m_eyeDir;
    ysVector m_eyeUp;

    ysVector3 m_doorLightPosition;
};

#endif /* APTSIM_APARTMENT_SIMULATOR_H */
