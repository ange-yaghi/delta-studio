#include "../include/asset_manager.h"

#include "../include/animation_export_file.h"
#include "../include/delta_engine.h"

#include <sys/stat.h>

dbasic::AssetManager::AssetManager() : ysObject("ASSET_MANAGER") {
    m_engine = NULL;
}

dbasic::AssetManager::~AssetManager() {
    /* void */
}

dbasic::Material *dbasic::AssetManager::NewMaterial() {
    Material *newMaterial = m_materials.NewGeneric<Material>();
    return newMaterial;
}

dbasic::Material *dbasic::AssetManager::FindMaterial(const char *name) {
    for (int i = 0; i < m_materials.GetNumObjects(); i++) {
        if (strcmp(m_materials.Get(i)->GetName(), name) == 0) return m_materials.Get(i);
    }

    return nullptr;
}

dbasic::SceneObjectAsset *dbasic::AssetManager::NewSceneObject() {
    SceneObjectAsset *newObject = m_sceneObjects.NewGeneric<SceneObjectAsset, 16>();
    newObject->m_manager = this;

    return newObject;
}

dbasic::SceneObjectAsset *dbasic::AssetManager::GetSceneObject(const char *name) {
    int objectCount = m_sceneObjects.GetNumObjects();

    for (int i = 0; i < objectCount; i++) {
        if (strcmp(m_sceneObjects.Get(i)->GetName(), name) == 0) {
            return m_sceneObjects.Get(i);
        }
    }

    return nullptr;
}

dbasic::SceneObjectAsset *dbasic::AssetManager::GetRoot(SceneObjectAsset *object) {
    dbasic::SceneObjectAsset *parent = GetSceneObject(object->GetParent());

    if (parent) return GetRoot(parent);
    else return object;
}

dbasic::ModelAsset *dbasic::AssetManager::NewModelAsset() {
    ModelAsset *newModelAsset = m_modelAssets.NewGeneric<ModelAsset, 16>();
    newModelAsset->m_manager = this;

    return newModelAsset;
}

dbasic::ModelAsset *dbasic::AssetManager::GetModelAsset(const char *name) {
    int modelCount = m_modelAssets.GetNumObjects();
    for (int i = 0; i < modelCount; i++) {
        if (strcmp(m_modelAssets.Get(i)->GetName(), name) == 0) {
            return m_modelAssets.Get(i);
        }
    }

    return nullptr;
}

ysError dbasic::AssetManager::CompileSceneFile(const char *fname, float scale, bool force) {
    YDS_ERROR_DECLARE("CompileSceneFile");

    char total_path[512];
    strcpy_s(total_path, 512, fname);

    ysToolGeometryFile toolFile;
    strcat_s(total_path, 512, ".ysc");
    YDS_NESTED_ERROR_CALL(toolFile.Open(total_path));

    strcpy_s(total_path, 512, fname);
    strcat_s(total_path, 512, ".ysce");

    if (toolFile.GetCompilationStatus() == ysToolGeometryFile::CompilationStatus::Compiled && !force) {
        // Check if the file actually exists
        struct stat buffer;
        if (stat(total_path, &buffer) == 0) {
            YDS_NESTED_ERROR_CALL(toolFile.Close());

            // File already exists, no compilation required
            return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
        }
    }

    ysGeometryExportFile exportFile;
    exportFile.Open(total_path);

    ysObjectData **objects = new ysObjectData * [toolFile.GetObjectCount()];
    int objectCount = toolFile.GetObjectCount();

    CompiledHeader header;
    header.ObjectCount = objectCount;

    YDS_NESTED_ERROR_CALL(exportFile.WriteCustomData(&header, sizeof(CompiledHeader)));

    for (int i = 0; i < objectCount; i++) {
        YDS_NESTED_ERROR_CALL(toolFile.ReadObject(&objects[i]));
        Material *material = FindMaterial(objects[i]->m_materialName);

        if (objects[i]->m_objectInformation.ObjectType == ysObjectData::ObjectType::Geometry) {
            ysGeometryPreprocessing::ResolveSmoothingGroupAmbiguity(objects[i]);
            ysGeometryPreprocessing::CreateAutomaticSmoothingGroups(objects[i]);
            ysGeometryPreprocessing::SeparateBySmoothingGroups(objects[i]);
            ysGeometryPreprocessing::CalculateNormals(objects[i]);

            if ((material != NULL) && material->UsesNormalMap())
                ysGeometryPreprocessing::CalculateTangents(objects[i], 0);

            if ((material != NULL) && (material->UsesNormalMap() || material->UsesSpecularMap() || material->UsesDiffuseMap())) {
                int startingVerts = objects[i]->m_objectStatistics.NumVertices;
                for (int ii = 0; ii < objects[i]->m_objectStatistics.NumUVChannels; ii++) {
                    ysGeometryPreprocessing::SeparateByUVGroups(objects[i], ii);
                }
            }

            ysGeometryPreprocessing::SortBoneWeights(objects[i]);
        }

        ysGeometryPreprocessing::CalculateNormals(objects[i]);
        ysGeometryPreprocessing::UniformScale(objects[i], scale);

        YDS_NESTED_ERROR_CALL(exportFile.WriteObject(objects[i]));
    }

    // Clear memory
    for (int i = 0; i < objectCount; i++) {
        delete objects[i];
    }

    delete[] objects;

    // Update compilation status
    YDS_NESTED_ERROR_CALL(toolFile.UpdateCompilationStatus(ysToolGeometryFile::CompilationStatus::Compiled));

    // Close files
    exportFile.Close();
    toolFile.Close();

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError dbasic::AssetManager::CompileInterchangeFile(const char *fname, float scale, bool force) {
    YDS_ERROR_DECLARE("CompileInterchangeFile");

    char completePath[512];
    strcpy_s(completePath, 512, fname);

    ysInterchangeFile0_1 toolFile;
    strcat_s(completePath, 512, ".dia");
    YDS_NESTED_ERROR_CALL(toolFile.Open(completePath));

    strcpy_s(completePath, 512, fname);
    strcat_s(completePath, 512, ".ysce");

    if (toolFile.GetCompilationStatus() && !force) {
        // Check if the file actually exists
        struct stat buffer;
        if (stat(completePath, &buffer) == 0) {
            YDS_NESTED_ERROR_CALL(toolFile.Close());

            // File already exists, no compilation required
            return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
        }
    }

    ysGeometryExportFile exportFile;
    exportFile.Open(completePath);

    ysInterchangeObject *objects = new ysInterchangeObject[toolFile.GetObjectCount()];
    int objectCount = toolFile.GetObjectCount();

    CompiledHeader header;
    header.ObjectCount = objectCount;

    YDS_NESTED_ERROR_CALL(exportFile.WriteCustomData(&header, sizeof(CompiledHeader)));

    for (int i = 0; i < objectCount; i++) {
        YDS_NESTED_ERROR_CALL(toolFile.ReadObject(&objects[i]));
        Material *material = FindMaterial(objects[i].MaterialName.c_str());

        ysGeometryExportFile::VertexInfo vertexInfo;
        if (objects[i].Type == ysInterchangeObject::ObjectType::Geometry) {
            if (vertexInfo.IncludeNormals) objects[i].RipByNormals();
            if (vertexInfo.IncludeTangents) objects[i].RipByTangents();
            if (vertexInfo.IncludeUVs) objects[i].RipByUVs();
        }

        objects[i].UniformScale(scale);

        YDS_NESTED_ERROR_CALL(exportFile.WriteObject(&objects[i]));
    }

    // Clear memory
    delete[] objects;

    // TODO: update compilation status

    // Close files
    exportFile.Close();
    toolFile.Close();

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError dbasic::AssetManager::LoadSceneFile(const char *fname, bool placeInVram) {
    char fullPath[512];
    strcpy_s(fullPath, 512, fname);
    strcat_s(fullPath, 512, ".ysce");

    std::fstream file(fullPath, std::ios::in | std::ios::binary);

    CompiledHeader fileHeader;
    file.read((char *)&fileHeader, sizeof(CompiledHeader));

    unsigned short *indicesFile = new unsigned short[2 * 1024 * 1024 / 2]; // 4 MB
    char *verticesFile = (char *)malloc(4 * 1024 * 1024); // 16 MB

    int currentIndexOffset = 0;
    int currentVertexByteOffset = 0;

    int initialIndex = m_sceneObjects.GetNumObjects();

    ysGPUBuffer *indexBuffer = nullptr;
    ysGPUBuffer *vertexBuffer = nullptr;

    if (placeInVram) {
        m_engine->GetDevice()->CreateIndexBuffer(&indexBuffer, 1 * 1024 * 1024, nullptr, false);
        m_engine->GetDevice()->CreateVertexBuffer(&vertexBuffer, 4 * 1024 * 1024, nullptr, false);
    }

    for (int i = 0; i < fileHeader.ObjectCount; i++) {
        ysGeometryExportFile::ObjectOutputHeader header;
        file.read((char *)&header, sizeof(ysGeometryExportFile::ObjectOutputHeader));

        SceneObjectAsset *newObject = NewSceneObject();

        ysObjectData::ObjectType objectType = static_cast<ysObjectData::ObjectType>(header.ObjectType);
        if (objectType == ysObjectData::ObjectType::Bone ||
            objectType == ysObjectData::ObjectType::Group ||
            objectType == ysObjectData::ObjectType::Plane ||
            objectType == ysObjectData::ObjectType::Instance ||
            objectType == ysObjectData::ObjectType::Empty) 
        {
            newObject->m_type = objectType;
            newObject->m_parent = (header.ParentIndex < 0) ? -1 : header.ParentIndex + initialIndex;
            newObject->m_skeletonIndex = header.SkeletonIndex;
            strcpy_s(newObject->m_name, 64, header.ObjectName);

            newObject->m_material = FindMaterial(header.ObjectMaterial);

            // Load Object Transformation
            ysVector translation = ysMath::LoadVector(header.Position, 1.0f);
            ysVector scale = ysMath::LoadVector(header.Scale);

            ysMatrix translationMatrix = ysMath::TranslationTransform(translation);
            ysMatrix scaleMatrix = ysMath::ScaleTransform(scale);

            ysVector x = ysMath::Constants::XAxis;
            ysVector y = ysMath::Constants::YAxis;
            ysVector z = ysMath::Constants::ZAxis;

            ysMatrix rotx = ysMath::RotationTransform(x, header.OrientationEuler.x * ysMath::Constants::PI / 180.0f);
            ysMatrix roty = ysMath::RotationTransform(y, header.OrientationEuler.y * ysMath::Constants::PI / 180.0f);
            ysMatrix rotz = ysMath::RotationTransform(z, header.OrientationEuler.z * ysMath::Constants::PI / 180.0f);

            newObject->ApplyTransformation(translationMatrix);

            newObject->ApplyTransformation(rotz);
            newObject->ApplyTransformation(roty);
            newObject->ApplyTransformation(rotx);

            newObject->ApplyTransformation(scaleMatrix);

            newObject->m_localOrientation = ysMath::LoadVector(header.Orientation);
            newObject->m_localPosition = translation;

            if (objectType == ysObjectData::ObjectType::Plane) {
                return YDS_ERROR_RETURN_MSG(ysError::YDS_UNSUPPORTED_TYPE, "Planes not supported.");
            }

            if (objectType == ysObjectData::ObjectType::Instance) {
                newObject->m_geometry = GetModelAsset(header.ParentInstanceIndex + initialIndex);
            }
        }
        else if (objectType == ysObjectData::ObjectType::Geometry) {
            // New model asset
            ModelAsset *newModelAsset = NewModelAsset();

            int vertexDataSize = header.VertexDataSize;
            int stride = header.VertexDataSize / header.NumVertices;

            if ((currentVertexByteOffset % stride) != 0) {
                currentVertexByteOffset += (stride - (currentVertexByteOffset % stride));
            }

            file.read((char *)(verticesFile + currentVertexByteOffset), vertexDataSize);
            file.read((char *)(indicesFile + currentIndexOffset), sizeof(unsigned short) * header.NumFaces * 3);

            if (header.NumBones > 0) {
                newModelAsset->m_boneMap.Preallocate(header.NumBones);

                for (int bone = 0; bone < header.NumBones; bone++) {
                    int &newBone = newModelAsset->m_boneMap.New();

                    file.read((char *)(&newBone), sizeof(int));
                    newBone += initialIndex;
                }
            }

            newObject->m_parent = (header.ParentIndex < 0) ? -1 : header.ParentIndex + initialIndex;
            newObject->m_type = ysObjectData::ObjectType::Geometry;

            newModelAsset->m_vertexSize = stride;
            newModelAsset->m_UVChannelCount = header.NumUVChannels;
            newModelAsset->m_vertexCount = header.NumVertices;
            newModelAsset->m_faceCount = header.NumFaces;
            newModelAsset->m_baseIndex = currentIndexOffset;
            newModelAsset->m_baseVertex = currentVertexByteOffset / stride;
            newModelAsset->m_vertexBuffer = vertexBuffer;
            newModelAsset->m_indexBuffer = indexBuffer;

            strcpy_s(newObject->m_name, 64, header.ObjectName);
            strcpy_s(newModelAsset->m_name, 64, header.ObjectName);

            newObject->m_material = FindMaterial(header.ObjectMaterial);
            newModelAsset->m_defaultMaterial = FindMaterial(header.ObjectMaterial);
            newObject->m_geometry = newModelAsset;

            currentIndexOffset += header.NumFaces * 3;
            currentVertexByteOffset += header.VertexDataSize;

            // Load Object Transformation
            ysVector translation = ysMath::LoadVector(header.Position);
            ysVector scale = ysMath::LoadVector(header.Scale);

            ysMatrix translationMatrix = ysMath::TranslationTransform(translation);
            ysMatrix scaleMatrix = ysMath::ScaleTransform(scale);

            ysVector x = ysMath::Constants::XAxis;
            ysVector y = ysMath::Constants::YAxis;
            ysVector z = ysMath::Constants::ZAxis;

            ysMatrix rotx = ysMath::RotationTransform(x, header.OrientationEuler.x * ysMath::Constants::PI / 180.0f);
            ysMatrix roty = ysMath::RotationTransform(y, header.OrientationEuler.y * ysMath::Constants::PI / 180.0f);
            ysMatrix rotz = ysMath::RotationTransform(z, header.OrientationEuler.z * ysMath::Constants::PI / 180.0f);

            newObject->ApplyTransformation(translationMatrix);

            newObject->ApplyTransformation(rotz);
            newObject->ApplyTransformation(roty);
            newObject->ApplyTransformation(rotx);

            newObject->ApplyTransformation(scaleMatrix);

            newObject->m_localOrientation = ysMath::LoadVector(header.Orientation);
            newObject->m_localPosition = translation;
        }
    }

    if (placeInVram) {
        m_engine->GetDevice()->EditBufferData(indexBuffer, (char *)indicesFile);
        m_engine->GetDevice()->EditBufferData(vertexBuffer, (char *)verticesFile);
    }

    delete[] indicesFile;
    free(verticesFile);

    file.close();

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError dbasic::AssetManager::CompileAnimationFile(const char *fname) {
    YDS_ERROR_DECLARE("CompileAnimationFile");

    char buffer[1024];
    sprintf_s(buffer, 1024, "%s.daf", fname);

    ysToolAnimationFile animationFile;

    YDS_NESTED_ERROR_CALL(animationFile.Open(buffer));

    ysExpandingArray<ysObjectAnimationData *> objectData;
    ysTimeTagData *timeTagData;

    for (int i = 0; i < animationFile.GetObjectCount(); i++) {
        YDS_NESTED_ERROR_CALL(animationFile.ReadObjectAnimation(&objectData.New()));
    }

    YDS_NESTED_ERROR_CALL(animationFile.ReadTimeTagData(&timeTagData));

    AnimationExportData exportAnimation;
    for (int i = 0; i < animationFile.GetObjectCount(); i++) {
        exportAnimation.LoadToolObjectData(objectData[i]);
    }

    exportAnimation.LoadTimeTagData(timeTagData);

    sprintf_s(buffer, 1024, "%s.dafc", fname);

    AnimationExportFile animationExportFile;
    animationExportFile.Open(buffer);
    animationExportFile.WriteObjectAnimationData(&exportAnimation);
    animationExportFile.Close();

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError dbasic::AssetManager::LoadAnimationFile(const char *fname) {
    YDS_ERROR_DECLARE("LoadAnimationFile");

    AnimationExportData *exportAnimationRead = m_animationExportData.New();
    AnimationExportFile animationExportFile;

    char buffer[1024];
    sprintf_s(buffer, 1024, "%s.dafc", fname);

    animationExportFile.Open(buffer, AnimationExportFile::Mode::Read);
    animationExportFile.ReadObjectAnimationData(exportAnimationRead);
    animationExportFile.Close();

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

dbasic::Skeleton *dbasic::AssetManager::BuildSkeleton(ModelAsset *model) {
    SceneObjectAsset *boneReference = NULL;
    SceneObjectAsset *boneParentReference = NULL;
    Bone *bone = NULL;
    int nBones = model->GetBoneCount();

    Skeleton *newSkeleton = m_skeletons.NewGeneric<Skeleton, 16>();

    // Get the root bone
    SceneObjectAsset *rootBoneReference = GetSceneObject(model->GetBoneMap(0));
    int nSceneObjects = GetSceneObjectCount();
    for (int i = 0; i < nSceneObjects; i++) {
        SceneObjectAsset *asset = GetSceneObject(i);
        if (GetRoot(asset) == rootBoneReference && asset->GetType() == ysObjectData::ObjectType::Bone) {
            newSkeleton->NewBone();
        }
    }

    for (int i = 0; i < nSceneObjects; i++) {
        SceneObjectAsset *asset = GetSceneObject(i);
        if (GetRoot(asset) == rootBoneReference && asset->GetType() == ysObjectData::ObjectType::Bone) {
            boneReference = GetSceneObject(i);

            bone = newSkeleton->GetBone(boneReference->m_skeletonIndex);
            bone->SetAssetID(i);
            bone->SetReferenceTransform(boneReference->GetWorldTransform());
            bone->RigidBody.SetOrientation(boneReference->GetLocalOrientation());
            bone->RigidBody.SetPosition(boneReference->GetPosition());
            bone->SetName(boneReference->m_name);

            if (asset == rootBoneReference) {
                newSkeleton->SetRootBone(bone);
            }
        }
    }

    nBones = newSkeleton->GetBoneCount();

    for (int i = 0; i < nBones; i++) {
        bone = newSkeleton->GetBone(i);
        boneReference = GetSceneObject(bone->GetAssetID());

        int parentIndex = boneReference->m_parent;
        boneParentReference = (parentIndex == -1) ? nullptr : GetSceneObject(parentIndex);

        if (boneParentReference != nullptr) {
            if (boneParentReference->m_skeletonIndex != -1) {
                bone->SetParent(newSkeleton->GetBone(boneParentReference->m_skeletonIndex));
            }
        }
        else {
            bone->SetParent(nullptr);
        }
    }

    return newSkeleton;
}

dbasic::RenderSkeleton *dbasic::AssetManager::BuildRenderSkeleton(dphysics::RigidBody *root, SceneObjectAsset *rootBone) {
    SceneObjectAsset *nodeReference = nullptr;
    SceneObjectAsset *nodeParentReference = nullptr;
    RenderNode *node = nullptr;

    RenderSkeleton *newRenderSkeleton = m_renderSkeletons.NewGeneric<RenderSkeleton, 16>();

    RenderNode *newNode = nullptr;
    newNode = newRenderSkeleton->NewNode();
    newNode->SetParent(nullptr);
    newNode->SetAssetID(rootBone->GetIndex());
    root->AddChild(&newNode->RigidBody);
    newNode->RigidBody.SetOrientation(rootBone->GetLocalOrientation());
    newNode->RigidBody.SetPosition(rootBone->GetPosition());
    newNode->SetModelAsset(rootBone->m_geometry);
    newNode->SetName(rootBone->m_name);

    // Get the root bone
    SceneObjectAsset *rootBoneReference = rootBone;

    ProcessRenderNode(rootBoneReference, newRenderSkeleton, nullptr, newNode);

    return newRenderSkeleton;
}

void dbasic::AssetManager::ProcessRenderNode(SceneObjectAsset *object, RenderSkeleton *skeleton, RenderNode *parent, RenderNode *top) {
    int nChildren = object->GetChildrenCount();
    RenderNode *newNode = nullptr;

    if (parent == nullptr) {
        newNode = top;
    }
    else {
        newNode = skeleton->NewNode();
        newNode->SetParent(parent);
        newNode->SetAssetID(object->GetIndex());
        newNode->RigidBody.SetOrientation(object->GetLocalOrientation());
        newNode->RigidBody.SetPosition(object->GetPosition());
        newNode->SetModelAsset(object->m_geometry);
        newNode->SetName(object->m_name);
    }

    for (int i = 0; i < nChildren; i++) {
        ProcessRenderNode(GetSceneObject(object->GetChild(i)), skeleton, newNode, top);
    }
}

dbasic::AnimationObjectController *dbasic::AssetManager::
    BuildAnimationObjectController(const char *name, dphysics::RigidBody *rigidBody)
{
    int nAnimationData = m_animationExportData.GetNumObjects();

    for (int i = 0; i < nAnimationData; i++) {
        int nObjects = m_animationExportData.Get(i)->GetKeyCount();

        for (int j = 0; j < nObjects; j++) {
            if (strcmp(name, m_animationExportData.Get(i)->GetKey(j)->GetObjectName()) == 0) {
                ObjectKeyframeDataExport *data = m_animationExportData.Get(i)->GetKey(j);

                int nKeys = data->GetKeyCount();

                AnimationObjectController *newController = m_animationControllers.New();
                newController->SetTarget(rigidBody);

                for (int k = 0; k < nKeys; k++) {
                    Keyframe *newKey = newController->AppendKeyframe();
                    newKey->LoadAssetKeyframe(data->GetKey(k));
                }

                return newController;
            }
        }
    }

    return NULL;
}

ysError dbasic::AssetManager::ResolveNodeHierarchy() {
    YDS_ERROR_DECLARE("ResolveNodeHierarchy");

    int nSceneObjects = GetSceneObjectCount();

    for (int i = 0; i < nSceneObjects; i++) {
        m_sceneObjects.Get(i)->ClearChildren();
    }

    for (int i = 0; i < nSceneObjects; i++) {
        int parent = m_sceneObjects.Get(i)->GetParent();

        if (parent != -1) {
            m_sceneObjects.Get(parent)->AddChild(i);
        }
    }

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}
