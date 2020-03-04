#include "../include/SceneManager.h"

SceneManager::SceneManager() {
    /* void */
}

SceneManager::~SceneManager() {
    /* void */
}

SceneObject *SceneManager::NewSceneObject() {
    SceneObject *newObject = m_sceneObjects.NewGeneric<SceneObject, 16>();
    newObject->m_manager = this;

    return newObject;
}

Material *SceneManager::NewMaterial() {
    Material *newMaterial = m_materials.NewGeneric<Material>();
    return newMaterial;
}

Material *SceneManager::FindMaterial(const char *name) {
    for (int i = 0; i < m_materials.GetNumObjects(); i++) {
        if (strcmp(m_materials.Get(i)->m_name, name) == 0) return m_materials.Get(i);
    }

    return NULL;
}

LightObject *SceneManager::FindLight(const char *name) {
    for (int i = 0; i < m_lights.GetNumObjects(); i++) {
        if (strcmp(m_lights.Get(i)->m_name, name) == 0) return m_lights.Get(i);
    }

    return NULL;
}

ShadowObject *SceneManager::FindShadow(const char *name) {
    for (int i = 0; i < m_shadows.GetNumObjects(); i++) {
        if (strcmp(m_shadows.Get(i)->m_name, name) == 0) return m_shadows.Get(i);
    }

    return NULL;
}

SceneObject *SceneManager::FindObject(const char *name) {
    for (int i = 0; i < m_sceneObjects.GetNumObjects(); i++) {
        if (strcmp(m_sceneObjects.Get(i)->m_name, name) == 0) return m_sceneObjects.Get(i);
    }

    return NULL;
}
