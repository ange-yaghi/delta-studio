#include "../include/ApartmentSimulator.h"

void ApartmentSimulator::GenerateOcclusionGraph() {
    SceneObject *volume;
    OcclusionVolume *newVolume;

    // Bathroom volume
    volume = m_sceneManager.FindObject("BathroomOccVolume");
    newVolume = m_sceneManager.m_occlusionVolumes.New();
    newVolume->m_volume = volume;
    newVolume->m_exclusions.New() = m_sceneManager.FindObject("ApartmentFrontSuperOccVolume");
    newVolume->m_exclusions.New() = m_sceneManager.FindObject("ApartmentRightSuperOccVolume");

    // Kitchen Rear Volume
    volume = m_sceneManager.FindObject("KitchenRearOccVolume");
    newVolume = m_sceneManager.m_occlusionVolumes.New();
    newVolume->m_volume = volume;
    newVolume->m_exclusions.New() = m_sceneManager.FindObject("ApartmentRearSuperOccVolume");
    newVolume->m_exclusions.New() = m_sceneManager.FindObject("HallwayOccVolume");
    newVolume->m_exclusions.New() = m_sceneManager.FindObject("BathroomOccVolume");

    // Kitchen Front Volume
    volume = m_sceneManager.FindObject("KitchenFrontOccVolume");
    newVolume = m_sceneManager.m_occlusionVolumes.New();
    newVolume->m_volume = volume;
    newVolume->m_exclusions.New() = m_sceneManager.FindObject("HallwayOccVolume");
    newVolume->m_exclusions.New() = m_sceneManager.FindObject("BathroomOccVolume");

    // Apartment Front Volume
    volume = m_sceneManager.FindObject("ApartmentFrontOccVolume");
    newVolume = m_sceneManager.m_occlusionVolumes.New();
    newVolume->m_volume = volume;
    newVolume->m_exclusions.New() = m_sceneManager.FindObject("BathroomOccVolume");

    // Apartment Front Volume
    volume = m_sceneManager.FindObject("ApartmentOccVolumeMid");
    newVolume = m_sceneManager.m_occlusionVolumes.New();
    newVolume->m_volume = volume;
    newVolume->m_exclusions.New() = m_sceneManager.FindObject("BathroomOccVolume");
    newVolume->m_exclusions.New() = m_sceneManager.FindObject("KitchenRearOccVolume");

    // Apartment Rear Volume
    volume = m_sceneManager.FindObject("ApartmentRearOccVolume");
    newVolume = m_sceneManager.m_occlusionVolumes.New();
    newVolume->m_volume = volume;
    newVolume->m_exclusions.New() = m_sceneManager.FindObject("KitchenRearOccVolume");

    // Apartment Rear Volume
    volume = m_sceneManager.FindObject("HallwayOccVolume");
    newVolume = m_sceneManager.m_occlusionVolumes.New();
    newVolume->m_volume = volume;
    newVolume->m_exclusions.New() = m_sceneManager.FindObject("FullKitchen");

    // Outside
    volume = m_sceneManager.FindObject("Outside");
    newVolume = m_sceneManager.m_occlusionVolumes.New();
    newVolume->m_volume = volume;
}

void ApartmentSimulator::GenerateCameraGraph() {
    m_cameraGraph.Clear();
    m_outside = false;

    for (int i = 0; i < m_sceneManager.m_occlusionVolumes.GetNumObjects(); i++) {
        if (m_sceneManager.m_occlusionVolumes.Get(i)->m_volume->Intersecting(m_currentEye)) {
            m_cameraGraph.New() = m_sceneManager.m_occlusionVolumes.Get(i);

            if (strcmp(m_sceneManager.m_occlusionVolumes.Get(i)->m_volume->m_name, "Outside") == 0) {
                m_outside = true;
            }
        }
    }
}
