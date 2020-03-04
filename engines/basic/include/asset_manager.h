#ifndef DELTA_BASIC_ASSET_MANAGER_H
#define DELTA_BASIC_ASSET_MANAGER_H

#include "delta_core.h"

#include "material.h"
#include "model_asset.h"
#include "scene_object_asset.h"
#include "skeleton.h"
#include "render_skeleton.h"
#include "animation_export_data.h"
#include "animation_object_controller.h"

namespace dbasic {

    struct CompiledHeader {
        int ObjectCount;
    };

    class DeltaEngine;

    class AssetManager : public ysObject {
    public:
        AssetManager();
        ~AssetManager();

        ysError CompileSceneFile(const char *fname, float scale = 1.0f, bool force = false);
        ysError LoadSceneFile(const char *fname);

        ysError CompileAnimationFile(const char *fname);
        ysError LoadAnimationFile(const char *fname);

        Material *NewMaterial();
        Material *FindMaterial(const char *name);
        Material *GetMaterial(int index) { return m_materials.Get(index); }

        SceneObjectAsset *NewSceneObject();
        int GetSceneObjectCount() const { return m_sceneObjects.GetNumObjects(); }
        SceneObjectAsset *GetSceneObject(int index) { return (index < 0) ? NULL : m_sceneObjects.Get(index); }
        SceneObjectAsset *GetSceneObject(const char *fname);
        SceneObjectAsset *GetRoot(SceneObjectAsset *object);

        ModelAsset *NewModelAsset();
        ModelAsset *GetModelAsset(int index) { return (index < 0) ? NULL : m_modelAssets.Get(index); }
        ModelAsset *GetModelAsset(const char *name);

        Skeleton *BuildSkeleton(ModelAsset *model);

        RenderSkeleton *BuildRenderSkeleton(RigidBody *root, SceneObjectAsset *rootBone);
        void ProcessRenderNode(SceneObjectAsset *asset, RenderSkeleton *skeleton, RenderNode *parent, RenderNode *top);

        AnimationObjectController *BuildAnimationObjectController(const char *name, RigidBody *rigidBody);

        void SetEngine(DeltaEngine *engine) { m_engine = engine; }

        ysError ResolveNodeHierarchy();

    protected:
        ysDynamicArray<ModelAsset, 4>				m_modelAssets;
        ysDynamicArray<SceneObjectAsset, 4>		m_sceneObjects;
        ysDynamicArray<Material, 4>				m_materials;
        ysDynamicArray<Skeleton, 4>				m_skeletons;
        ysDynamicArray<RenderSkeleton, 4>			m_renderSkeletons;
        ysDynamicArray<AnimationObjectController, 4>	m_animationControllers;
        ysDynamicArray<AnimationExportData, 4>	m_animationExportData;

        DeltaEngine *m_engine;
    };

} /* namespace dbasic */

#endif /* DELTA_BASIC_ASSET_MANAGER_H */
