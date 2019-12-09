#ifndef APTSIM_SCENE_MANAGER_H
#define APTSIM_SCENE_MANAGER_H

#include "SceneObject.h"
#include "Lighting.h"

#include "../../../include/yds_expanding_array.h"

class SceneManager {
public:
    SceneManager();
    ~SceneManager();

    SceneObject *NewSceneObject();

    SceneObject *FindObject(const char *name);
    SceneObject *GetObject(int index) { return m_sceneObjects.Get(index); }

    LightObject *NewLight() { return m_lights.New(); }
    LightObject *FindLight(const char *name);
    LightVolume *NewLightVolume() { return m_lightVolumes.New(); }
    ShadowObject *NewShadow() { return m_shadows.New(); }
    ShadowObject *FindShadow(const char *name);

    Material *NewMaterial();
    Material *FindMaterial(const char *name);
    Material *GetMaterial(int index) { return m_materials.Get(index); }

    int GetNextIndex() const { return m_sceneObjects.GetNumObjects(); }
    int GetObjectCount() const { return m_sceneObjects.GetNumObjects(); }
    SceneObject *GetIndex(int index) { return (index >= 0) ? m_sceneObjects.Get(index) : NULL; }

    int NumLights() { return m_lights.GetNumObjects(); }
    int NumShadows() { return m_shadows.GetNumObjects(); }
    int NumLightVolumes() { return m_lightVolumes.GetNumObjects(); }

    ysDynamicArray<OcclusionVolume, 4> m_occlusionVolumes;

    ysDynamicArray<LightVolume, 4> m_lightVolumes;

    ysDynamicArray<LightObject, 4> m_lights;
    ysDynamicArray<ShadowObject, 4> m_shadows;

protected:
    ysDynamicArray<SceneObject, 4> m_sceneObjects;
    ysDynamicArray<Material, 4> m_materials;
};

#endif /* APTSIM_SCENE_MANAGER_H */
