#include "../include/ApartmentSimulator.h"

void ApartmentSimulator::SubmitLight(LightObject *light, int index) {
    Light *gpuLight = &m_lightData.Lights[index];

    gpuLight->Ambient = light->m_ambient;
    gpuLight->Color = light->m_color;
    gpuLight->LowColor = light->m_lowColor;
    gpuLight->CutoffAmbient = light->m_cutoffAmbient;
    gpuLight->CutoffHigh = light->m_cutoffHigh;
    gpuLight->CutoffLow = light->m_cutoffLow;
    gpuLight->CutoffMix = light->m_cutoffMix;
    gpuLight->CutoffPower = light->m_cutoffPower;
    gpuLight->Diffuse = light->m_diffuse;
    gpuLight->Direction = light->m_direction;
    gpuLight->Falloff = light->m_falloff;
    gpuLight->Intensity = light->m_intensity;
    gpuLight->Location = light->m_location;
    gpuLight->Specular = light->m_specular;
    gpuLight->SpecularPower = light->m_specularPower;
    gpuLight->AuxCutoffAmount = light->m_auxCutoffAmount;
    gpuLight->AuxCutoffDirection = light->m_auxCutoffDirection;
}

void ApartmentSimulator::SubmitShadow(ShadowObject *shadow, int index) {
    if (index >= NUM_SHADOWS) {
        return;
    }

    PlaneShadow *gpuShadow = &m_lightData.Shadows[index];

    gpuShadow->Falloff = shadow->m_falloff;
    gpuShadow->FringeMask = shadow->m_fringeMask;
    gpuShadow->HalfDepth = shadow->m_depth / 2;
    gpuShadow->HalfHeight = shadow->m_height / 2;
    gpuShadow->HalfWidth = shadow->m_width / 2;
    gpuShadow->Location = shadow->m_location;
    gpuShadow->ShadowIntensity = shadow->m_shadowIntensity;
}

void ApartmentSimulator::InitializeLighting() {
    ysVector4 incandescent = ysVector4(235.0f / 255, 225.0f / 255, 142.0f / 255);

    LightObject *light;
    LightVolume *volume;
    SceneObject *obj;

    // KitchenLight
    light = m_sceneManager.NewLight();
    m_kitchenLight = light;
    light->SetName("KitchenLight");
    light->m_color = incandescent;
    light->m_direction = ysVector4(0.0f, -1.0f, 0.0f);
    light->m_cutoffLow = 0.9f;
    light->m_cutoffHigh = 1.1f;
    light->m_cutoffPower = 2.0f;
    light->m_cutoffMix = 1.0f;

    light->m_diffuse = 0.8f; //0.6
    light->m_ambient = 0.5f; //0.5
    light->m_specular = 0.4f; //0.4
    light->m_specularPower = 64.0f;

    light->m_falloff = 2 * 12.0f; // 2 ft
    light->m_intensity = 1.0f;
    light->m_location = ysVector4(-5.646f * 12, 7.1f * 12, -7.4f * 12);

    volume = m_sceneManager.NewLightVolume();
    volume->m_volumes.New() = m_sceneManager.FindObject("KitchenLightVolume1");
    volume->m_volumes.New() = m_sceneManager.FindObject("KitchenLightVolume2");
    volume->m_volumes.New() = m_sceneManager.FindObject("RoomVolume");

    light->m_volume = volume;

    // Create Door Light
    light = m_sceneManager.NewLight();
    light->SetName("DoorLight");
    light->m_color = incandescent;
    light->m_direction = ysVector4(0, -1, 0);
    light->m_cutoffLow = 0.8f;
    light->m_cutoffHigh = 1.1f;
    light->m_cutoffPower = 2.0f;
    light->m_cutoffMix = 1.0f;

    light->m_diffuse = 0.3f;
    light->m_ambient = 0.7f;
    light->m_specular = 0.05f;
    light->m_specularPower = 8.0f;

    light->m_falloff = 1.5f * 12; // 2 ft
    light->m_intensity = 0.75f;
    light->m_location = m_doorLightPosition;

    m_doorLight = light;

    volume = m_sceneManager.NewLightVolume();
    volume->m_volumes.New() = m_sceneManager.FindObject("RoomVolume");

    light->m_volume = volume;

    // Create Exterior Light 1
    obj = m_sceneManager.FindObject("ExteriorLightLeft");
    obj->m_hidden = true;
    light = m_sceneManager.NewLight();
    light->SetName("ExteriorLightLeft");
    light->m_color = ysVector4(167.0f / 255, 219.0f / 255, 255.0f / 255);
    light->m_direction = ysMath::GetVector3(ysMath::Normalize(ysMath::LoadVector(0.0f, -0.2f, 1.0f)));
    light->m_cutoffLow = 1.5f;
    light->m_cutoffHigh = 0.5f;
    light->m_cutoffPower = 4.0f;
    light->m_cutoffMix = 0.0f;

    light->m_diffuse = 0.0f; // 0.3
    light->m_ambient = 0.7f; // 0.7
    light->m_specular = 0.75f; // 0.1
    light->m_specularPower = 64.0f;
    light->m_on = true;

    light->m_falloff = 5 * 12.0f; // 30 ft
    light->m_intensity = 0.75f; //2.0
    light->m_location = obj->GetPosition();
    m_exteriorLights[0] = light;

    volume = m_sceneManager.NewLightVolume();
    volume->m_volumes.New() = m_sceneManager.FindObject("RoomVolume");
    volume->m_volumes.New() = m_sceneManager.FindObject("KitchenLightVolume1");
    volume->m_volumes.New() = m_sceneManager.FindObject("KitchenLightVolume2");
    volume->m_volumes.New() = m_sceneManager.FindObject("HallwayRegion");

    light->m_volume = volume;

    // Create Exterior Light 2
    obj = m_sceneManager.FindObject("ExteriorLightRight");
    obj->m_hidden = true;
    light = m_sceneManager.NewLight();
    light->SetName("ExteriorLightRight");
    light->m_color = ysVector4(167.0f / 255, 219.0f / 255, 255.0f / 255);
    light->m_direction = ysMath::GetVector3(ysMath::Normalize(ysMath::LoadVector(0.0f, -0.2f, 1.0f)));
    light->m_cutoffLow = 1.5f;
    light->m_cutoffHigh = 0.5f;
    light->m_cutoffPower = 4.0f;
    light->m_cutoffMix = 0.0f;

    light->m_diffuse = 0.0f; // 0.3
    light->m_ambient = 0.7f; // 0.7
    light->m_specular = 0.75f; // 0.1
    light->m_specularPower = 64.0f;
    light->m_on = true;

    light->m_falloff = 5 * 12.0f; // 30 ft
    light->m_intensity = 0.75f; //2.0
    light->m_location = obj->GetPosition(); ysVector4(30 * 12.0f, 4 * 12.0f, -40 * 12.0f); // -20
    m_exteriorLights[1] = light;

    light->m_volume = volume;

    // Room light
    light = m_sceneManager.NewLight();
    light->SetName("RoomLight");
    light->m_color = ysVector4(255.0f / 255, 237.0f / 255, 177.0f / 255);//ysVector4(138/255.0, 219/255.0, 255/255.0);
    light->m_diffuse = 0.1f; // 0.1
    light->m_ambient = 0.5f; // 0.5
    light->m_specular = 0.0f;
    light->m_specularPower = 4.0f;

    light->m_direction = ysVector4(0, 0, 0);
    light->m_cutoffLow = 1.5f;
    light->m_cutoffHigh = 1.5f;
    light->m_cutoffPower = 1.0f;
    light->m_cutoffMix = 0.0f;

    light->m_falloff = 20.0f * 12; // 100 ft
    light->m_intensity = 0.5f; //0.5
    light->m_location = ysVector4(4 * 12.0f, 4 * 12.0f, 0); // 0	
    //light->m_volume = volume;
    m_roomAmbient = light;

    // Bathroom Light Bulbs

    ysVector4 incandescent2 = ysVector4(235.0f / 255, 235.0f / 255, 200.0f / 255);

    volume = m_sceneManager.NewLightVolume();
    volume->m_volumes.New() = m_sceneManager.FindObject("BathroomVolume");
    LightVolume *bathroomVolume = volume;

    LightObject *masterbulb = m_sceneManager.NewLight();
    masterbulb->m_virtual = true;
    masterbulb->SetName("BathroomLightBulb");
    masterbulb->m_falloff = 1.5f * 12; // 1 ft

    masterbulb->m_direction = ysVector4(-1.0f, 0.0f, 0.0f, 0.0f);
    masterbulb->m_cutoffLow = 1.5f;
    masterbulb->m_cutoffHigh = 1.5f;
    masterbulb->m_cutoffPower = 1.0f;
    masterbulb->m_cutoffMix = 0.0f;

    masterbulb->m_intensity = 0.15f;
    masterbulb->m_diffuse = 0.3f; // 0.6
    masterbulb->m_ambient = 0.9f; // 0.6
    masterbulb->m_specular = 0.6f; // 0.4
    masterbulb->m_specularPower = 128.0f;
    masterbulb->m_color = incandescent;

    char bulbName[64];
    for (int i = 0; i < 5; i++) {
        sprintf_s(bulbName, 64, "Lightbulb%d", i);

        SceneObject *object = m_sceneManager.FindObject(bulbName);
        //if (i%2) continue;

        light = m_sceneManager.NewLight();
        light->SetName(bulbName);
        light->m_location = object->GetPosition();
        light->m_volume = volume;
        light->m_reference = masterbulb;

        m_bathroomLights[i] = light;
    }

    light = m_sceneManager.NewLight();
    light->SetName("BathroomAmbientLight");
    light->m_direction = ysVector4(-1.0, 0.0, 0.0, 0.0);
    light->m_location = m_sceneManager.FindObject("BathroomAmbientLight")->GetPosition();
    light->m_falloff = 2.0f * 12; // 1.5 ft
    light->m_cutoffLow = 1.5f;
    light->m_cutoffHigh = 1.5f;
    light->m_cutoffPower = 1.0f;
    light->m_cutoffMix = 0.0f;

    light->m_intensity = 1.0f;
    light->m_diffuse = 0.5f;
    light->m_ambient = 0.8f;
    light->m_specular = 0.25f;
    light->m_specularPower = 128.0f;
    light->m_color = incandescent;
    //light->m_on = false;

    light->m_volume = volume;
    m_bathroomLights[5] = light;

    // Create Lamp Show Light
    light = m_sceneManager.NewLight();
    light->SetName("LampShowLight");
    light->m_color = ysVector4(255.0f / 255.0f, 204.0f / 255, 128.0f / 255);
    light->m_direction = ysVector4(0, 1, 0);
    light->m_cutoffLow = 0.75f;
    light->m_cutoffHigh = 0.75f;
    light->m_cutoffPower = 4.0f;
    light->m_cutoffMix = 0.0f;

    light->m_diffuse = 0.9f; // 0.5
    light->m_ambient = 0.0f; // 0.5
    light->m_specular = 0.1f; // 0.2
    light->m_specularPower = 8.0f;

    light->m_falloff = 1 * 12.0f; // 30 ft
    light->m_intensity = 0.75f; //2.0
    light->m_location = m_sceneManager.FindObject("LargeLampBulb")->GetPosition();

    volume = m_sceneManager.NewLightVolume();
    volume->m_volumes.New() = m_sceneManager.FindObject("RoomVolume");
    //volume->m_volumes.New() = m_sceneManager.FindObject("KitchenLightVolume1");
    //volume->m_volumes.New() = m_sceneManager.FindObject("KitchenLightVolume2");
    //volume->m_volumes.New() = m_sceneManager.FindObject("HallwayRegion");

    light->m_volume = volume;
    m_lampLights[0] = light;

    // Create Lamp Power Light
    light = m_sceneManager.NewLight();
    light->SetName("LampPowerLight");
    light->m_color = ysVector4(255.0f / 255, 204.0f / 255, 128.0f / 255);
    light->m_direction = ysVector4(1, 0, 0);
    light->m_cutoffLow = 0.2f;
    light->m_cutoffHigh = 1.5f;
    light->m_cutoffPower = 4.0f;
    light->m_cutoffMix = 1.0f;

    light->m_diffuse = 0.5f; // 0.5
    light->m_ambient = 0.2f; // 0.5
    light->m_specular = 0.1f; // 0.2
    light->m_specularPower = 8.0f;

    light->m_falloff = 4 * 12.0f; // 30 ft
    light->m_intensity = 1.0f; //2.0
    light->m_location = m_sceneManager.FindObject("LargeLampBulb")->GetPosition();

    volume = m_sceneManager.NewLightVolume();
    volume->m_volumes.New() = m_sceneManager.FindObject("RoomVolume");
    volume->m_volumes.New() = m_sceneManager.FindObject("KitchenLightVolume1");
    volume->m_volumes.New() = m_sceneManager.FindObject("KitchenLightVolume2");
    //volume->m_volumes.New() = m_sceneManager.FindObject("HallwayRegion");

    light->m_volume = volume;
    m_lampLights[1] = light;

    obj = m_sceneManager.FindObject("Monitor1Screen");
    volume = m_sceneManager.NewLightVolume();
    volume->m_volumes.New() = m_sceneManager.FindObject("RoomVolume");
    volume->m_volumes.New() = m_sceneManager.FindObject("KitchenLightVolume2");
    light = m_sceneManager.NewLight();
    light->SetName("ComputerScreen");
    light->m_location = obj->GetPosition();
    light->m_volume = volume;

    volume = m_sceneManager.NewLightVolume();
    volume->m_volumes.New() = m_sceneManager.FindObject("HallwayRegion");
    obj = m_sceneManager.FindObject("HallwayLightBulb");
    light = m_sceneManager.NewLight();
    light->SetName("HallwayLight");
    light->m_location = obj->GetPosition();
    light->m_volume = volume;

    // --------------------------------------------
    // Create Shadows

    ShadowObject *shadow;

    // Bed
    obj = m_sceneManager.FindObject("BedShadow");
    volume = m_sceneManager.NewLightVolume();
    volume->m_volumes.New() = m_sceneManager.FindObject("BedShadowVolume");

    shadow = m_sceneManager.NewShadow();
    shadow->m_location = ysVector4(66.0f, 1.0f, 14.0f);
    shadow->m_falloff = 6;
    shadow->m_shadowIntensity = 2.0;
    shadow->m_fringeMask = ysVector4(1.0, 3.0, 1.0);
    shadow->m_depth = obj->m_length;
    shadow->m_width = obj->m_width;
    shadow->m_volume = volume;

    obj = m_sceneManager.FindObject("BedWallShadow");
    shadow = m_sceneManager.NewShadow();
    shadow->m_location = obj->GetPosition();
    shadow->m_falloff = 6;
    shadow->m_shadowIntensity = 1.0;
    shadow->m_fringeMask = ysVector4(1.0, 1.0, 1.0);
    shadow->m_depth = obj->m_length;
    shadow->m_height = obj->m_width;
    shadow->m_volume = volume;

    // Night Table Floor
    obj = m_sceneManager.FindObject("NightTableShadow");

    shadow = m_sceneManager.NewShadow();
    shadow->m_location = obj->GetPosition();
    shadow->m_falloff = 2;
    shadow->m_shadowIntensity = 1.5;
    shadow->m_fringeMask = ysVector4(1.0, 5.0, 1.0);
    shadow->m_depth = obj->m_length;
    shadow->m_width = obj->m_width;
    shadow->m_volume = volume;

    // Night Table Wall
    obj = m_sceneManager.FindObject("NightTableWallShadow");

    shadow = m_sceneManager.NewShadow();
    shadow->m_location = obj->GetPosition();
    shadow->m_falloff = 2;
    shadow->m_shadowIntensity = 1.0;
    shadow->m_fringeMask = ysVector4(1.0, 1.0, 1.0);
    shadow->m_depth = obj->m_length;
    shadow->m_width = 0;
    shadow->m_height = obj->m_width;
    shadow->m_volume = volume;

    // Counter 1
    volume = m_sceneManager.NewLightVolume();
    volume->m_volumes.New() = m_sceneManager.FindObject("SouthCounterShadowVolume");

    shadow = m_sceneManager.NewShadow();
    shadow->m_location = ysVector4(-(8 * 12 + 3), 0, -(6 * 12 + 8));
    shadow->m_falloff = 0.25 * 12;
    shadow->m_fringeMask = ysVector4(1.0, 1.0, 2);
    shadow->m_shadowIntensity = 0.6f;
    shadow->m_depth = (float)(5 * 12 + 9);
    shadow->m_width = (float)(1 * 12 + 10);
    shadow->m_volume = volume;

    // Counter 2
    volume = m_sceneManager.NewLightVolume();
    volume->m_volumes.New() = m_sceneManager.FindObject("WestCounterShadowVolume");

    shadow = m_sceneManager.NewShadow();
    shadow->m_location = ysVector4(-(5 * 12 + 2), 0, -(4 * 12 + 1));
    shadow->m_fringeMask = ysVector4(1000.0, 1.0, 1);
    shadow->m_falloff = 0.25f * 12;
    shadow->m_shadowIntensity = 0.6f;
    shadow->m_width = (float)(5 * 12 + 8);
    shadow->m_depth = (float)(1 * 12 + 10);
    shadow->m_volume = volume;

    // Radiators
    volume = m_sceneManager.NewLightVolume();
    volume->m_volumes.New() = m_sceneManager.FindObject("RadiatorShadowVolume");

    shadow = m_sceneManager.NewShadow();
    shadow->m_location = ysVector4(-(2 * 12 + 4.5), 0.5, -(11 * 12 + 0));
    shadow->m_falloff = 0.1f * 12;
    shadow->m_shadowIntensity = 4.0f;
    shadow->m_width = (float)(12 * 12 + 1);
    shadow->m_depth = (float)(0 * 12 + 2);
    shadow->m_volume = volume;

    // Bathroom
    shadow = m_sceneManager.NewShadow();
    shadow->m_location = ysVector4(-(3 * 12 + 4.5), 0.5, (0 * 12 + 7));
    shadow->m_falloff = 0.5f * 12;
    shadow->m_shadowIntensity = 1.3f;
    shadow->m_depth = (float)(4 * 12 + 6);
    shadow->m_width = (float)(1 * 12 + 1);
    shadow->m_volume = bathroomVolume;

    // Large Closet Floor
    volume = m_sceneManager.NewLightVolume();
    volume->m_volumes.New() = m_sceneManager.FindObject("LargeClosetShadowVolume");

    shadow = m_sceneManager.NewShadow();
    shadow->m_location = ysVector4((float)(5 * 12 + 3), 0.5f, (float)(8 * 12 + 1));
    shadow->m_falloff = 0.5f * 12;
    shadow->m_fringeMask = ysVector4(10.0f, 1.0f, 1.0f);
    shadow->m_shadowIntensity = 1.5f;
    shadow->m_width = (float)(6 * 12 + 17);
    shadow->m_depth = (float)(1 * 12 + 6);
    shadow->m_volume = volume;

    // Large Closet Ceiling
    shadow = m_sceneManager.NewShadow();
    shadow->m_location = ysVector4((5 * 12 + 4), 8 * 12, (8 * 12 + 3));
    shadow->m_fringeMask = ysVector4(10.0f, 0.8f, 4.0f);
    shadow->m_falloff = 0.5 * 12;
    shadow->m_shadowIntensity = 1.0;
    shadow->m_height = 2;
    shadow->m_width = (6 * 12 + 18);
    shadow->m_depth = (1 * 12 + 18);
    shadow->m_volume = volume;

    // Microwave Shadow
    obj = m_sceneManager.FindObject("MicrowaveShadow");
    volume = m_sceneManager.NewLightVolume();
    volume->m_volumes.New() = m_sceneManager.FindObject("MicrowaveShadowVolume");

    shadow = m_sceneManager.NewShadow();
    shadow->m_location = obj->GetPosition();
    shadow->m_fringeMask = ysVector4(1.0, 10.0, 1.0);
    shadow->m_falloff = 1;
    shadow->m_shadowIntensity = 5;
    shadow->m_height = 0;
    shadow->m_width = obj->m_width;
    shadow->m_depth = obj->m_length;
    shadow->m_volume = volume;

    // Fridge Top
    obj = m_sceneManager.FindObject("FridgeTopShadow");
    volume = m_sceneManager.NewLightVolume();
    volume->m_volumes.New() = m_sceneManager.FindObject("FridgeTopShadowVolume");

    shadow = m_sceneManager.NewShadow();
    shadow->SetName("FridgeTop");
    shadow->m_location = obj->GetPosition();
    shadow->m_fringeMask = ysVector4(5.0, 1.0, 10.0);
    shadow->m_falloff = 1 * 12;
    shadow->m_shadowIntensity = 1.0;
    shadow->m_height = 5 * 12;
    shadow->m_width = obj->m_width;
    shadow->m_depth = obj->m_length;
    shadow->m_volume = volume;

    // Fridge Bottom
    obj = m_sceneManager.FindObject("FridgeShadow");
    volume = m_sceneManager.NewLightVolume();
    volume->m_volumes.New() = m_sceneManager.FindObject("FridgeShadowVolume");

    shadow = m_sceneManager.NewShadow();
    shadow->m_location = obj->GetPosition();
    shadow->m_fringeMask = ysVector4(1.0, 10.0, 1.0);
    shadow->m_falloff = 1;
    shadow->m_shadowIntensity = 3.0;
    shadow->m_height = 0;
    shadow->m_width = obj->m_width;
    shadow->m_depth = obj->m_length;
    shadow->m_volume = volume;

    // Fridge Handle
    obj = m_sceneManager.FindObject("FridgeHandleShadow");
    volume = m_sceneManager.NewLightVolume();
    volume->m_volumes.New() = m_sceneManager.FindObject("FridgeHandleShadowVolume");

    shadow = m_sceneManager.NewShadow();
    shadow->m_location = obj->GetPosition();
    shadow->m_fringeMask = ysVector4(1.0, 5.0, 1.0);
    shadow->m_falloff = 0.2f;
    shadow->m_shadowIntensity = 3.0f;
    shadow->m_height = 0;
    shadow->m_width = obj->m_width;
    shadow->m_depth = obj->m_length;
    shadow->m_volume = volume;

    // Fridge Door
    obj = m_sceneManager.FindObject("FridgeDoorShadow");

    shadow = m_sceneManager.NewShadow();
    shadow->SetName("FridgeDoorShadow");
    shadow->m_location = obj->GetPosition();
    shadow->m_fringeMask = ysVector4(1.0, 5.0, 1.0);
    shadow->m_falloff = 0.2f;
    shadow->m_shadowIntensity = 3.0f;
    shadow->m_height = obj->m_length;
    shadow->m_width = 0;
    shadow->m_depth = obj->m_width;
    shadow->m_volume = volume;

    // Smoke Detector
    obj = m_sceneManager.FindObject("SmokeDetectorShadow");
    volume = m_sceneManager.NewLightVolume();
    volume->m_volumes.New() = m_sceneManager.FindObject("SmokeDetectorShadowVolume");

    shadow = m_sceneManager.NewShadow();
    shadow->m_location = obj->GetPosition();
    shadow->m_fringeMask = ysVector4(1.0, 2.0, 1.0);
    shadow->m_falloff = 1;
    shadow->m_shadowIntensity = 0.5;
    shadow->m_height = 0;
    shadow->m_width = obj->m_width;
    shadow->m_depth = obj->m_length;
    shadow->m_volume = volume;

    // Lamp
    obj = m_sceneManager.FindObject("LampShadow");
    volume = m_sceneManager.NewLightVolume();
    volume->m_volumes.New() = m_sceneManager.FindObject("LampShadowVolume");

    shadow = m_sceneManager.NewShadow();
    shadow->m_location = obj->GetPosition();
    shadow->m_fringeMask = ysVector4(1.0, 10.0, 1.0);
    shadow->m_falloff = 2;
    shadow->m_shadowIntensity = 3.0;
    shadow->m_height = 0;
    shadow->m_width = obj->m_width;
    shadow->m_depth = obj->m_length;
    shadow->m_volume = volume;

    // Desk

    obj = m_sceneManager.FindObject("DeskShadow");
    volume = m_sceneManager.NewLightVolume();
    volume->m_volumes.New() = m_sceneManager.FindObject("DeskShadowVolume");

    shadow = m_sceneManager.NewShadow();
    shadow->m_location = obj->GetPosition();
    shadow->m_fringeMask = ysVector4(1.0, 1.0, 1.0);
    shadow->m_falloff = 1 * 12;
    shadow->m_shadowIntensity = 1.5;
    shadow->m_height = 0;
    shadow->m_width = obj->m_width;
    shadow->m_depth = obj->m_length;
    shadow->m_volume = volume;

    // Cabinet South
    obj = m_sceneManager.FindObject("CabinetSouthShadow");
    volume = m_sceneManager.NewLightVolume();
    volume->m_volumes.New() = m_sceneManager.FindObject("CabinetWestShadowVolume");
    volume->m_volumes.New() = m_sceneManager.FindObject("CabinetSouthShadowVolume");

    shadow = m_sceneManager.NewShadow();
    shadow->m_location = obj->GetPosition();
    shadow->m_fringeMask = ysVector4(1.0, 1.0, 1.0);
    shadow->m_falloff = 3;
    shadow->m_shadowIntensity = 1.5;
    shadow->m_height = 0;
    shadow->m_width = obj->m_width;
    shadow->m_depth = obj->m_length;
    shadow->m_volume = volume;

    // Cabinet South
    obj = m_sceneManager.FindObject("CabinetWestShadow");

    shadow = m_sceneManager.NewShadow();
    shadow->m_location = obj->GetPosition();
    shadow->m_fringeMask = ysVector4(1.0, 1.0, 1.0);
    shadow->m_falloff = 3;
    shadow->m_shadowIntensity = 1.5;
    shadow->m_height = 0;
    shadow->m_width = obj->m_width;
    shadow->m_depth = obj->m_length;
    shadow->m_volume = volume;

    // Cabinet South 2
    obj = m_sceneManager.FindObject("CabinetWestShadow001");

    shadow = m_sceneManager.NewShadow();
    shadow->m_location = obj->GetPosition();
    shadow->m_fringeMask = ysVector4(1.0, 1.0, 1.0);
    shadow->m_falloff = 3;
    shadow->m_shadowIntensity = 1.5;
    shadow->m_height = 0;
    shadow->m_width = obj->m_width;
    shadow->m_depth = obj->m_length;
    shadow->m_volume = volume;

    // Sunlight
    obj = m_sceneManager.FindObject("SunlightShadow");
    volume = m_sceneManager.NewLightVolume();
    volume->m_volumes.New() = m_sceneManager.FindObject("SunlightShadowVolume");

    shadow = m_sceneManager.NewShadow();
    shadow->m_location = obj->GetPosition();
    shadow->m_fringeMask = ysVector4(1.0, 1.0, 1.0);
    shadow->m_falloff = 0.5;
    shadow->m_shadowIntensity = 1.0;
    shadow->m_height = 0;
    shadow->m_width = obj->m_width;
    shadow->m_depth = obj->m_length;
    shadow->m_volume = volume;

    // Small Closet
    obj = m_sceneManager.FindObject("SmallClosetShadow");
    volume = m_sceneManager.NewLightVolume();
    volume->m_volumes.New() = m_sceneManager.FindObject("SmallClosetShadowVolume");
    volume->m_volumes.New() = m_sceneManager.FindObject("HallwayRegion");

    shadow = m_sceneManager.NewShadow();
    shadow->m_location = obj->GetPosition();
    shadow->m_fringeMask = ysVector4(1.0, 1.0, 1.0);
    shadow->m_falloff = 1 * 12;
    shadow->m_shadowIntensity = 2.0;
    shadow->m_height = 8 * 12;
    shadow->m_width = obj->m_width;
    shadow->m_depth = obj->m_length;
    shadow->m_volume = volume;

    obj = m_sceneManager.FindObject("ClosetBoxShadow1");
    shadow = m_sceneManager.NewShadow();
    shadow->m_location = obj->GetPosition();
    shadow->m_fringeMask = ysVector4(1.0, 2.0, 1.0);
    shadow->m_falloff = 2;
    shadow->m_shadowIntensity = 1.0;
    shadow->m_height = 0;
    shadow->m_width = obj->m_width;
    shadow->m_depth = obj->m_length;
    shadow->m_volume = volume;

    obj = m_sceneManager.FindObject("ClosetBoxShadow2");
    shadow = m_sceneManager.NewShadow();
    shadow->m_location = obj->GetPosition();
    shadow->m_fringeMask = ysVector4(1.0, 2.0, 1.0);
    shadow->m_falloff = 2;
    shadow->m_shadowIntensity = 1.0;
    shadow->m_height = 0;
    shadow->m_width = obj->m_width;
    shadow->m_depth = obj->m_length;
    shadow->m_volume = volume;

    obj = m_sceneManager.FindObject("ClosetDoorShadow");
    shadow = m_sceneManager.NewShadow();
    shadow->m_location = obj->GetPosition();
    shadow->m_fringeMask = ysVector4(1.0, 5.0, 1.0);
    shadow->m_falloff = 3;
    shadow->m_shadowIntensity = 1.0;
    shadow->m_height = 0;
    shadow->m_width = obj->m_width;
    shadow->m_depth = obj->m_length;
    shadow->m_volume = volume;

    // Main Door
    obj = m_sceneManager.FindObject("MainDoorShadow");
    volume = m_sceneManager.NewLightVolume();
    volume->m_volumes.New() = m_sceneManager.FindObject("MainDoorShadowVolume");

    shadow = m_sceneManager.NewShadow();
    shadow->m_location = obj->GetPosition();
    shadow->m_fringeMask = ysVector4(1.0, 1.0, 1.0);
    shadow->m_falloff = 6;
    shadow->m_shadowIntensity = 0.75;
    shadow->m_height = 0 * 12;
    shadow->m_width = obj->m_width;
    shadow->m_depth = obj->m_length;
    shadow->m_volume = volume;

    // Drape Shadow
    obj = m_sceneManager.FindObject("DrapeShadow");
    volume = m_sceneManager.NewLightVolume();
    volume->m_volumes.New() = m_sceneManager.FindObject("DrapeShadowVolume");

    shadow = m_sceneManager.NewShadow();
    shadow->m_location = obj->GetPosition();
    shadow->m_fringeMask = ysVector4(1.0, 1.0, 1.0);
    shadow->m_falloff = 2;
    shadow->m_shadowIntensity = 0.25;
    shadow->m_height = 0 * 12;
    shadow->m_width = obj->m_width;
    shadow->m_depth = obj->m_length;
    shadow->m_volume = volume;

    // Corner Shadow
    obj = m_sceneManager.FindObject("CornerShadow");
    volume = m_sceneManager.NewLightVolume();
    volume->m_volumes.New() = m_sceneManager.FindObject("CornerShadowVolume");

    shadow = m_sceneManager.NewShadow();
    shadow->m_location = obj->GetPosition();
    shadow->m_fringeMask = ysVector4(1.0, 1.0, 1.0);
    shadow->m_falloff = 6;
    shadow->m_shadowIntensity = 0.3f;
    shadow->m_height = 8 * 12;
    shadow->m_width = 0;
    shadow->m_depth = 0;
    shadow->m_volume = volume;

    // Sliding Panels
    obj = m_sceneManager.FindObject("BathroomPanelsShadow");
    volume = m_sceneManager.NewLightVolume();
    volume->m_volumes.New() = m_sceneManager.FindObject("BathroomPanelsShadowVolume");

    shadow = m_sceneManager.NewShadow();
    shadow->m_location = obj->GetPosition();
    shadow->m_fringeMask = ysVector4(1.0, 1.0, 0.5);
    shadow->m_falloff = 0.05f;
    shadow->m_shadowIntensity = 1.0;
    shadow->m_height = obj->m_length;
    shadow->m_width = 0;
    shadow->m_depth = obj->m_width;
    shadow->m_volume = volume;

    // Bathroom Mirror Shadow
    obj = m_sceneManager.FindObject("BathroomMirrorShadow");
    volume = m_sceneManager.NewLightVolume();
    volume->m_volumes.New() = m_sceneManager.FindObject("BathroomVolume");

    shadow = m_sceneManager.NewShadow();
    shadow->m_location = obj->GetPosition();
    shadow->m_fringeMask = ysVector4(2.0, 1.0, 1.0);
    shadow->m_falloff = 2;
    shadow->m_shadowIntensity = 1.0;
    shadow->m_height = obj->m_width;
    shadow->m_width = 0;
    shadow->m_depth = obj->m_length;
    shadow->m_volume = volume;

    // Bathroom Hoop
    obj = m_sceneManager.FindObject("BathroomHoopShadow");
    volume = m_sceneManager.NewLightVolume();
    volume->m_volumes.New() = m_sceneManager.FindObject("BathroomHoopShadowVolume");

    shadow = m_sceneManager.NewShadow();
    shadow->m_location = obj->GetPosition();
    shadow->m_fringeMask = ysVector4(1.0, 1.0, 2.0);
    shadow->m_falloff = 2;
    shadow->m_shadowIntensity = 0.25;
    shadow->m_height = obj->m_length;
    shadow->m_width = obj->m_width;;
    shadow->m_depth = 0;
    shadow->m_volume = volume;

    // Oven Shadows
    volume = m_sceneManager.NewLightVolume();
    volume->m_volumes.New() = m_sceneManager.FindObject("OvenShadowVolume001");
    volume->m_volumes.New() = m_sceneManager.FindObject("OvenShadowVolume002");

    obj = m_sceneManager.FindObject("OvenShadowLeft");
    shadow = m_sceneManager.NewShadow();
    shadow->m_location = obj->GetPosition();
    shadow->m_fringeMask = ysVector4(5.0, 1.0, 1.0);
    shadow->m_falloff = 2.0;
    shadow->m_shadowIntensity = 2.0;
    shadow->m_height = obj->m_length;
    shadow->m_width = 1;
    shadow->m_depth = obj->m_width;
    shadow->m_volume = volume;

    obj = m_sceneManager.FindObject("OvenShadowRight");
    shadow = m_sceneManager.NewShadow();
    shadow->m_location = obj->GetPosition();
    shadow->m_fringeMask = ysVector4(2.0, 0.75, 0.75);
    shadow->m_falloff = 1;
    shadow->m_shadowIntensity = 2.0;
    shadow->m_height = obj->m_length;
    shadow->m_width = 0.5;
    shadow->m_depth = obj->m_width;
    shadow->m_volume = volume;

    obj = m_sceneManager.FindObject("OvenShadowBack");
    shadow = m_sceneManager.NewShadow();
    shadow->m_location = obj->GetPosition();
    shadow->m_fringeMask = ysVector4(1.0, 1.0, 2.0);
    shadow->m_falloff = 1.0;
    shadow->m_shadowIntensity = 2.0;
    shadow->m_height = obj->m_length;
    shadow->m_width = obj->m_width;
    shadow->m_depth = 0;
    shadow->m_volume = volume;

    // Weight Shadows
    volume = m_sceneManager.NewLightVolume();
    volume->m_volumes.New() = m_sceneManager.FindObject("WeightsShadowVolume");

    obj = m_sceneManager.FindObject("WeightsLeftShadow");
    shadow = m_sceneManager.NewShadow();
    shadow->m_location = obj->GetPosition();
    shadow->m_fringeMask = ysVector4(1.0, 1.0, 2.0);
    shadow->m_falloff = 3.0;
    shadow->m_shadowIntensity = 2.0;
    shadow->m_height = 0;
    shadow->m_width = obj->m_length;
    shadow->m_depth = obj->m_width;
    shadow->m_volume = volume;

    obj = m_sceneManager.FindObject("WeightsRightShadow");
    shadow = m_sceneManager.NewShadow();
    shadow->m_location = obj->GetPosition();
    shadow->m_fringeMask = ysVector4(1.0, 1.0, 2.0);
    shadow->m_falloff = 3.0;
    shadow->m_shadowIntensity = 2.0;
    shadow->m_height = 0;
    shadow->m_width = obj->m_length;
    shadow->m_depth = obj->m_width;
    shadow->m_volume = volume;

    // Alarm Clock Shadow
    volume = m_sceneManager.NewLightVolume();
    volume->m_volumes.New() = m_sceneManager.FindObject("AlarmClockShadowVolume");

    obj = m_sceneManager.FindObject("AlarmClockShadow");
    shadow = m_sceneManager.NewShadow();
    shadow->m_location = obj->GetPosition();
    shadow->m_fringeMask = ysVector4(1.0, 10.0, 1.0);
    shadow->m_falloff = 1.0;
    shadow->m_shadowIntensity = 2.0;
    shadow->m_height = 0;
    shadow->m_width = obj->m_length;
    shadow->m_depth = obj->m_width;
    shadow->m_volume = volume;

    // Bathroom Thing Shadow
    volume = m_sceneManager.NewLightVolume();
    volume->m_volumes.New() = m_sceneManager.FindObject("BathroomThingShadowVolume");

    obj = m_sceneManager.FindObject("BathroomThingShadowLeft");
    shadow = m_sceneManager.NewShadow();
    shadow->SetName("BathroomThingShadowLeft");
    shadow->m_location = obj->GetPosition();
    shadow->m_fringeMask = ysVector4(1.0, 1.0, 1.0);
    shadow->m_falloff = 0.5;
    shadow->m_shadowIntensity = 0.5;
    shadow->m_height = obj->m_length;
    shadow->m_width = 0;
    shadow->m_depth = obj->m_width;
    shadow->m_volume = volume;

    obj = m_sceneManager.FindObject("BathroomThingShadowRight");
    shadow = m_sceneManager.NewShadow();
    shadow->SetName("BathroomThingShadowRight");
    shadow->m_location = obj->GetPosition();
    shadow->m_fringeMask = ysVector4(1.0, 1.0, 1.0);
    shadow->m_falloff = 0.5;
    shadow->m_shadowIntensity = 0.5;
    shadow->m_height = obj->m_length;
    shadow->m_width = 0;
    shadow->m_depth = obj->m_width;
    shadow->m_volume = volume;

    // Door Light Shadow
    volume = m_sceneManager.NewLightVolume();
    volume->m_volumes.New() = m_sceneManager.FindObject("DoorLightShadowVolume");
    obj = m_sceneManager.FindObject("DoorLightShadow");
    shadow = m_sceneManager.NewShadow();
    shadow->SetName("DoorLightShadow");
    shadow->m_location = obj->GetPosition();
    shadow->m_fringeMask = ysVector4(1.0, 2.0, 1.0);
    shadow->m_falloff = 1;
    shadow->m_shadowIntensity = 0.5;
    shadow->m_height = 0;
    shadow->m_width = obj->m_width;
    shadow->m_depth = obj->m_length;
    shadow->m_volume = volume;

    // Hall Light Shadow
    volume = m_sceneManager.NewLightVolume();
    volume->m_volumes.New() = m_sceneManager.FindObject("HallLightShadowVolume");
    obj = m_sceneManager.FindObject("HallLightShadow");
    shadow = m_sceneManager.NewShadow();
    shadow->SetName("HallLightShadow");
    shadow->m_location = obj->GetPosition();
    shadow->m_fringeMask = ysVector4(1.0, 2.0, 1.0);
    shadow->m_falloff = 1;
    shadow->m_shadowIntensity = 5.5;
    shadow->m_height = 0;
    shadow->m_width = obj->m_width;
    shadow->m_depth = obj->m_length;
    shadow->m_volume = volume;
}
