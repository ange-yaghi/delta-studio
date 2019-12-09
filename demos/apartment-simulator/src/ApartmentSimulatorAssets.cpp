#include "../include/ApartmentSimulator.h"

#define PATH "E:/Engine Project/SpadinaApartment/"
//#define PATH ""

void ApartmentSimulator::CreateMaterials() {
    ysTexture *woodFloorDiffuse;
    ysTexture *woodFloorSpecular;
    ysTexture *woodFloorNormal;
    ysTexture *tileFloorDiffuse;
    ysTexture *ceilingDiffuse;
    ysTexture *closetComposite;
    ysTexture *bedWood;
    ysTexture *counterTop;
    ysTexture *kitchenWallTiles;
    ysTexture *kitchenWallTilesSpecular;
    ysTexture *ceilingNormal;
    ysTexture *tileFloorNormal;
    ysTexture *wallTileNormal;
    ysTexture *wallDiffuse;
    ysTexture *wallNormal;
    ysTexture *radiatorDiffuse;
    ysTexture *radiatorNormal;

    ysTexture *brickDiffuse;
    ysTexture *brickNormal;
    ysTexture *balconyWallDiffuse;
    ysTexture *balconyWallSpecular;
    ysTexture *balconyWallNormal;

    ysTexture *balconySideWallDiffuse;
    ysTexture *balconySideWallNormal;

    ysTexture *balconyFloorDiffuse;
    ysTexture *balconyFloorSpecular;

    ysTexture *balconyCeilingDiffuse;
    ysTexture *balconyCeilingNormal;

    ysTexture *bathroomFloorDiffuse;
    ysTexture *bathroomFloorNormal;

    ysTexture *bathroomCounterDiffuse;

    ysTexture *cloud;
    ysTexture *tieDye;

    ysTexture *diffuse;
    ysTexture *normal;

    Material *material;

    UpdateTitleScreen(0);

    m_device->CreateTexture(&cloud, GetAssetPath("textures/Steel/Cloud.png").c_str());
    m_device->CreateTexture(&tieDye, GetAssetPath("textures/Steel/BigDains.png").c_str());

    // Wood floor
    m_device->CreateTexture(&woodFloorDiffuse, GetAssetPath("textures/WoodFloor/Tiled.jpg").c_str());
    m_device->CreateTexture(&woodFloorSpecular, GetAssetPath("textures/WoodFloor/TiledSpecular.jpg").c_str());
    m_device->CreateTexture(&woodFloorNormal, GetAssetPath("textures/WoodFloor/WoodFloorNormal.jpg").c_str());
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "WoodFloor");
    material->m_diffuseMap = woodFloorDiffuse;
    material->m_specularMap = woodFloorSpecular;
    material->m_normalMap = woodFloorNormal;

    // Tiled floor
    m_device->CreateTexture(&tileFloorDiffuse, GetAssetPath("textures/Tiles/TiledFloorSmall.png").c_str());
    m_device->CreateTexture(&tileFloorNormal, GetAssetPath("textures/Tiles/TiledFloorNormal.png").c_str());
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "TileFloor");
    material->m_diffuseMap = tileFloorDiffuse;
    material->m_normalMap = tileFloorNormal;
    material->m_useFalloff = 0;

    // Ceiling
    m_device->CreateTexture(&ceilingDiffuse, GetAssetPath("textures/Ceiling/CeilingTiled.png").c_str());
    m_device->CreateTexture(&ceilingNormal, GetAssetPath("textures/Ceiling/CeilingNormal.png").c_str());
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "Ceiling");
    material->m_diffuseMap = ceilingDiffuse;
    material->m_normalMap = ceilingNormal;
    material->m_specular = 0.4f;

    UpdateTitleScreen(0.1f);

    // Large Closet
    m_device->CreateTexture(&closetComposite, GetAssetPath("textures/LargeCloset/ClosetComposite.png").c_str());
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "LargeCloset");
    material->m_diffuseMap = closetComposite;
    material->m_lit = 1;

    // Sky 
    m_device->CreateTexture(&m_sky1, GetAssetPath("textures/Outside/FinalPanoramas/Compressed/Panorama3PM_PartlyCloudy.png").c_str());
    m_device->CreateTexture(&m_sky2, GetAssetPath("textures/Outside/FinalPanoramas/Compressed/Panorama7PM_Cloudy.png").c_str());
    m_device->CreateTexture(&m_sky3, GetAssetPath("textures/Outside/FinalPanoramas/Compressed/Panorama11PM.png").c_str());
    m_device->CreateTexture(&m_sky4, GetAssetPath("textures/Outside/FinalPanoramas/Compressed/Panorama7AM_Snow.png").c_str());
    //m_sky2 = m_sky3 = m_sky4 = NULL;
    m_skyMaterial = m_sceneManager.NewMaterial();
    strcpy_s(m_skyMaterial->m_name, 64, "Sky");
    m_skyMaterial->m_diffuseMap = m_sky1;
    m_skyMaterial->m_lit = 0;

    UpdateTitleScreen(0.2f);

    // Fridge White
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "FridgeWhite");
    material->m_diffuseColor = ysVector4(1.0, 1.0, 1.0, 1.0);
    material->m_specular = 1.0;
    //material->m_specularPower = 2;
    m_fridgeWhite = material;

    // Fridge Yellow
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "FridgeYellow");
    material->m_diffuseColor = ysVector4(255.0f / 255, 255.0f / 255, 235.0f / 255, 1.0f);
    material->m_specular = 1.0;
    material->m_specularPower = 1;

    // Grey Steel
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "GreySteel");
    material->m_diffuseColor = ysVector4(255.0f / 255, 255.0f / 255, 255.0f / 255, 1.0f);
    material->m_specular = 1.5;
    material->m_specularPower = 0.125;
    material->m_reflectMap = cloud;
    material->m_reflectPower = 1.0;
    m_chrome = material;

    // Tie Dye
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "TieDye");
    material->m_diffuseColor = ysVector4(200.0f / 255, 200.0f / 255, 200.0f / 255, 1.0f);
    material->m_specular = 1.0;
    material->m_specularPower = 1.0;
    material->m_reflectMap = tieDye;
    material->m_reflectPower = 1.0;
    m_tieDyeMaterial = material;

    UpdateTitleScreen(0.3f);

    // Light Gold
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "LightGold");
    material->m_diffuseColor = ysVector4(255.0f / 255, 255.0f / 255, 235.0f / 255, 1.0f);
    material->m_specular = 2.0;
    material->m_specularPower = 1;
    material->m_reflectMap = cloud;
    material->m_reflectPower = 0.5;

    // Black Paint
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "BlackPaint");
    material->m_diffuseColor = ysVector4(50.0f / 255, 50.0f / 255, 50.0f / 255, 1.0f);
    material->m_specular = 1.5;
    material->m_specularPower = 2;
    material->m_useFalloff = 0;
    material->m_falloffPower = 1;
    material->m_reflectMap = cloud;

    // Rubber
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "Rubber");
    material->m_diffuseColor = ysVector4(100.0f / 255, 100.0f / 255, 100.0f / 255, 1.0f);
    material->m_specular = 0.5;
    material->m_specularPower = 1.0f;
    material->m_reflectMap = cloud;
    material->m_reflectPower = 0.5f;

    // Bed Wood
    m_device->CreateTexture(&bedWood, GetAssetPath("textures/Bed/Wood.jpg").c_str());
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "BedWood");
    material->m_diffuseMap = bedWood;
    material->m_specular = 0.5f;
    material->m_specularPower = 1.0f;
    material->m_useFalloff = 0;
    material->m_falloffPower = 1.0f;

    // Counter Top
    m_device->CreateTexture(&counterTop, GetAssetPath("textures/Counter/CounterTop.png").c_str());
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "CounterTop");
    material->m_diffuseMap = counterTop;
    material->m_specular = 0.5f;
    material->m_specularPower = 1.0f;
    material->m_useFalloff = 0;
    material->m_falloffPower = 1.0f;

    // Kitchen Wall Tiling
    m_device->CreateTexture(&kitchenWallTiles, GetAssetPath("textures/WallTiles/KitchenWallTiles.png").c_str());
    m_device->CreateTexture(&kitchenWallTilesSpecular, GetAssetPath("textures/WallTiles/KitchenWallTilesSpecular.png").c_str());
    m_device->CreateTexture(&wallTileNormal, GetAssetPath("textures/WallTiles/KitchenWallTilesNormal.png").c_str());
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "KitchenWallTiling");
    material->m_diffuseMap = kitchenWallTiles;
    material->m_specularMap = kitchenWallTilesSpecular;
    material->m_normalMap = wallTileNormal;
    material->m_specular = 0.7f;
    material->m_ambient = 1.0f;

    material->m_specularPower = 1.0f;
    material->m_useFalloff = 0;
    material->m_falloffPower = 1.0f;

    UpdateTitleScreen(0.4f);

    // Wall
    m_device->CreateTexture(&wallDiffuse, GetAssetPath("textures/Wall/WallDiffuse.png").c_str());
    m_device->CreateTexture(&wallNormal, GetAssetPath("textures/Wall/WallNormal.png").c_str());
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "Wall");
    material->m_diffuseMap = wallDiffuse;
    material->m_normalMap = wallNormal;

    // Radiator
    m_device->CreateTexture(&radiatorDiffuse, GetAssetPath("textures/Radiator/Radiator.png").c_str());
    m_device->CreateTexture(&radiatorNormal, GetAssetPath("textures/Radiator/RadiatorNormal.png").c_str());
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "Radiator");
    material->m_diffuseMap = radiatorDiffuse;
    material->m_normalMap = radiatorNormal;

    // Exterior Bronze Sheet
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "ExteriorBronzeSheet");
    material->m_diffuseColor = ysVector4(100 / 255.0f, 78 / 244.0f, 60 / 255.0f);
    material->m_reflectMap = cloud;

    // Balcony Half Wall
    m_device->CreateTexture(&balconyWallDiffuse, GetAssetPath("textures/Balcony/BalconyWall.png").c_str());
    m_device->CreateTexture(&balconyWallSpecular, GetAssetPath("textures/Balcony/BalconyWallSpecular.png").c_str());
    m_device->CreateTexture(&balconyWallNormal, GetAssetPath("textures/Balcony/BalconyWallNormal.png").c_str());
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "BalconyWall");
    material->m_diffuseMap = balconyWallDiffuse;
    material->m_specularMap = balconyWallSpecular;
    material->m_normalMap = balconyWallNormal;

    // Brick
    m_device->CreateTexture(&brickDiffuse, GetAssetPath("textures/Balcony/Brick.png").c_str());
    m_device->CreateTexture(&brickNormal, GetAssetPath("textures/Balcony/BrickNormal.png").c_str());
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "Brick");
    material->m_diffuseMap = brickDiffuse;
    material->m_normalMap = brickNormal;

    // Balcony Side Wall
    m_device->CreateTexture(&balconySideWallDiffuse, GetAssetPath("textures/Balcony/BalconySideWall.png").c_str());
    m_device->CreateTexture(&balconySideWallNormal, GetAssetPath("textures/Balcony/BalconySideWallNormal.png").c_str());
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "BalconySideWall");
    material->m_diffuseMap = balconySideWallDiffuse;
    material->m_normalMap = balconySideWallNormal;

    UpdateTitleScreen(0.5f);

    // Balcony Floor
    m_device->CreateTexture(&balconyFloorDiffuse, GetAssetPath("textures/Balcony/BalconyFloor.png").c_str());
    m_device->CreateTexture(&balconyFloorSpecular, GetAssetPath("textures/Balcony/BalconyFloorSpecular.png").c_str());
    //m_device->CreateTexture(&wallTileNormal, PATH "Assets/Balcony/KitchenWallTilesNormal.png");
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "BalconyFloor");
    material->m_diffuseMap = balconyFloorDiffuse;
    material->m_specularMap = balconyFloorSpecular;
    material->m_normalMap = NULL;
    material->m_diffuseColor = ysVector4(0.8f, 0.8f, 0.8f, 1.0f);
    material->m_specular = 0.7f;
    material->m_ambient = 1.0f;

    // Balcony Ceiling
    m_device->CreateTexture(&balconyCeilingDiffuse, GetAssetPath("textures/Balcony/BalconyCeiling.png").c_str());
    m_device->CreateTexture(&balconyCeilingNormal, GetAssetPath("textures/Balcony/BalconyCeilingNormal.png").c_str());
    //m_device->CreateTexture(&wallTileNormal, PATH "Assets/Balcony/KitchenWallTilesNormal.png");
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "BalconyCeiling");
    material->m_diffuseMap = balconyCeilingDiffuse;
    material->m_normalMap = balconyCeilingNormal;
    material->m_specular = 0.7f;
    material->m_ambient = 1.0f;

    // Drapes
    m_device->CreateTexture(&balconyCeilingDiffuse, GetAssetPath("textures/Drapes/Drapes.tif").c_str());
    //m_device->CreateTexture(&wallTileNormal, PATH "Assets/Balcony/KitchenWallTilesNormal.png");
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "Drapes");
    material->m_diffuseMap = balconyCeilingDiffuse;
    material->m_specular = 0.1f;
    material->m_ambient = 0.3f;
    material->m_useAlpha = 1;

    // Bathroom Floor
    m_device->CreateTexture(&bathroomFloorDiffuse, GetAssetPath("textures/BathroomFloor/BathroomFloor.png").c_str());
    m_device->CreateTexture(&bathroomFloorNormal, GetAssetPath("textures/BathroomFloor/BathroomFloorNormal.png").c_str());
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "BathroomFloor");
    material->m_diffuseMap = bathroomFloorDiffuse;
    material->m_normalMap = bathroomFloorNormal;
    material->m_ambient = 1.0f;

    // Bathroom Counter
    m_device->CreateTexture(&bathroomCounterDiffuse, GetAssetPath("textures/BathroomCounter/BathroomCounter.png").c_str());
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "BathroomCounter");
    material->m_diffuseMap = bathroomCounterDiffuse;
    material->m_ambient = 1.0f;

    // Microwave
    m_device->CreateTexture(&diffuse, GetAssetPath("textures/Microwave/FinalTexture.jpg").c_str());
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "Microwave");
    material->m_diffuseMap = diffuse;
    material->m_ambient = 1.0f;

    // Bed
    m_device->CreateTexture(&diffuse, GetAssetPath("textures/Bed/Sheet.jpg").c_str());
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "BedSheet");
    material->m_diffuseMap = diffuse;
    material->m_specular = 0.0f;

    UpdateTitleScreen(0.6f);

    // Generic Bed
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "GenericBed");
    material->m_diffuseColor = ysVector4(33.0f / 255, 49.0f / 255, 99.0f / 255);
    material->m_specular = 0.0f;

    // Lamp Shade
    m_device->CreateTexture(&diffuse, GetAssetPath("textures/Lamp/Lamp.png").c_str());
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "LampShade");
    material->m_diffuseMap = diffuse;
    material->m_specular = 0.0f;
    material->m_lit = 0;
    m_lampMaterial = material;

    // Light Bulb
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "LightBulb");
    material->m_specular = 0.0f;
    material->m_lit = 0;
    m_lightBulb = material;
    material->m_lit = 0;
    //ysVector4(235/255.0, 225/255.0, 142/255.0);
    //ysVector4(255/255.0, 204/255.0, 128/255.0);
    material->m_diffuseColor = ysVector4(255.0f / 255, 235.0f / 255, 159.0f / 255);
    material->m_falloffColor = ysVector4(255.0f / 255, 184.0f / 255, 132.0f / 255);
    material->m_useFalloff = 1;
    material->m_falloffPower = 4.0f;

    // Light Bulb Casing
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "LightBulbCasing");
    material->m_specular = 0.0f;
    material->m_lit = 0;
    m_lightBulbCasing = material;
    material->m_diffuseColor = ysVector4(255.0f / 255, 235.0f / 255, 159.0f / 255);
    material->m_falloffColor = ysVector4(255.0f / 255, 200.0f / 255, 100.0f / 255);
    material->m_useFalloff = 1;
    material->m_falloffPower = 1.0f;

    UpdateTitleScreen(0.7f);

    // Sunlight
    m_device->CreateTexture(&diffuse, GetAssetPath("textures/SunLight/Sunlight.png").c_str());
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "Sunlight");
    material->m_diffuseMap = diffuse;

    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "SunlightYellow");
    material->m_diffuseColor = ysVector4(255.0f / 255, 258.0f / 255, 45.0f / 255);

    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "SunlightBlue");
    material->m_diffuseColor = ysVector4(43.0f / 255, 52.0f / 255, 87.0f / 255);

    // Mirowave LCD
    material = m_microwaveClock.m_onMaterial = m_ovenClock.m_onMaterial = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "MicrowaveLCDOn");
    material->m_diffuseColor = ysVector4(101 / 255.0f, 227 / 255.0f, 28 / 255.0f);
    material->m_lit = 0;

    material = m_microwaveClock.m_offMaterial = m_ovenClock.m_offMaterial = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "MicrowaveLCDOff");
    material->m_diffuseColor = ysVector4(43 / 255.0f, 43 / 255.0f, 23 / 255.0f);
    material->m_specular = 0.0f;

    // Alarm Clock LCD
    material = m_alarmClock.m_onMaterial = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "AlarmClockLCDOn");
    material->m_diffuseColor = ysVector4(255 / 255.0f, 172 / 255.0f, 20 / 255.0f);
    material->m_lit = 0;

    material = m_alarmClock.m_offMaterial = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "AlarmClockLCDOff");
    material->m_diffuseColor = ysVector4(81 / 255.0f, 23 / 255.0f, 0 / 255.0f);
    material->m_specular = 0.0f;

    UpdateTitleScreen(0.8f);

    // Grey Rubber
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "GreyRubber");
    material->m_diffuseColor = ysVector4(100 / 255.0f, 100 / 255.0f, 100 / 255.0f);
    material->m_specular = 1.0f;

    // Bath Tiles
    m_device->CreateTexture(&diffuse, GetAssetPath("textures/BathroomTiles/BathTiles.png").c_str());
    m_device->CreateTexture(&normal, GetAssetPath("textures/BathroomTiles/BathTilesNormal.png").c_str());
    //m_device->CreateTexture(&wallTileNormal, PATH "Assets/Balcony/KitchenWallTilesNormal.png");
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "BathTiles");
    material->m_diffuseMap = diffuse;
    material->m_normalMap = normal;
    material->m_specular = 1.0f;
    material->m_ambient = 1.0f;

    // Bathroom Misc.
    m_device->CreateTexture(&diffuse, GetAssetPath("textures/BathroomMisc/BathroomMisc.png").c_str());
    m_device->CreateTexture(&normal, GetAssetPath("textures/BathroomMisc/BathroomMiscNormal.png").c_str());
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "BathroomMisc");
    material->m_diffuseMap = diffuse;
    material->m_normalMap = normal;
    material->m_specular = 1.0f;
    material->m_ambient = 1.0f;

    // Porcelain
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "Porcelain");
    material->m_diffuseColor = ysVector4(255 / 255.0, 255 / 255.0, 255 / 255.0);
    material->m_specular = 1.0f;
    material->m_reflectMap = cloud;
    material->m_reflectPower = 0.06125f;

    UpdateTitleScreen(0.9f);

    // Cardboard Boxes
    m_device->CreateTexture(&diffuse, GetAssetPath("textures/Boxes/BoxesComposite.png").c_str());
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "CardboardBox");
    material->m_diffuseMap = diffuse;
    material->m_specular = 0.0f;

    // Night Table
    m_device->CreateTexture(&diffuse, GetAssetPath("textures/NightTable/NightTable.png").c_str());
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "NightTable");
    material->m_diffuseMap = diffuse;

    // Monitor 1
    m_device->CreateTexture(&diffuse, GetAssetPath("textures/Computer/ComputerScreen.png").c_str());
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "Monitor1");
    material->m_diffuseMap = diffuse;
    material->m_lit = 0;

    // Gold
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "Gold");
    material->m_diffuseColor = ysVector4(247 / 255.0f, 163 / 255.0f, 0 / 255.0f, 1.0f);
    material->m_specular = 2.0f;
    material->m_specularPower = 1.0f;
    material->m_reflectMap = cloud;
    material->m_reflectPower = 0.25f;

    // Fire Alarm
    m_device->CreateTexture(&diffuse, GetAssetPath("textures/FireAlarm/FireAlarm.png").c_str());
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "FireAlarm");
    material->m_diffuseColor = ysVector4(255 / 255.0f, 255 / 255.0f, 255 / 255.0f, 1.0f);
    material->m_diffuseMap = diffuse;
    material->m_specular = 1.0f;
    material->m_specularPower = 1.0f;

    // Filament
    m_device->CreateTexture(&diffuse, GetAssetPath("textures/OvenFilament/Paint.png").c_str());
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "OvenFilament");
    material->m_diffuseColor = ysVector4(255 / 255.0, 255 / 255.0, 255 / 255.0, 1.0);
    material->m_diffuseMap = diffuse;
    material->m_reflectMap = cloud;
    material->m_specular = 1.0;
    material->m_specularPower = 1;

    // Oven Panel
    m_device->CreateTexture(&diffuse, GetAssetPath("textures/Oven/OvenPanel.jpg").c_str());
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "OvenPanel");
    material->m_diffuseColor = ysVector4(255 / 255.0f, 255 / 255.0f, 255 / 255.0f, 1.0f);
    material->m_diffuseMap = diffuse;
    material->m_specular = 1.0;
    material->m_specularPower = 1;

    // Normal Mapping Test
    material = m_sceneManager.NewMaterial();
    strcpy_s(material->m_name, 64, "NormalTest");
    material->m_normalMap = ceilingNormal;
    material->m_specular = 0.2f;
    material->m_ambient = 1.0f;

    material->m_specularPower = 1.0f;
    material->m_useFalloff = 0;
    material->m_falloffPower = 1.0f;

    UpdateTitleScreen(1.0f);
}

void ApartmentSimulator::CompileSceneAssets() {
    float increment = 1 / 25.0f;
    int i = 1;

    UpdateTitleScreen(increment * i++);
    //CompileSceneFile("Assets/Models/test");

    UpdateTitleScreen(increment * i++);
    //CompileSceneFile("models/apartment");

    UpdateTitleScreen(increment * i++);
    //CompileSceneFile("models/large_closet");

    UpdateTitleScreen(increment * i++);
    //CompileSceneFile("models/fridge");

    UpdateTitleScreen(increment * i++);
    //CompileSceneFile("models/environment");

    UpdateTitleScreen(increment * i++);
    //CompileSceneFile("models/balcony");

    UpdateTitleScreen(increment * i++);
    //CompileSceneFile("models/kitchen_cabinets");

    UpdateTitleScreen(increment * i++);
    //CompileSceneFile("models/radiators");

    UpdateTitleScreen(increment * i++);
    //CompileSceneFile("models/door_frames");

    UpdateTitleScreen(increment * i++);
    //CompileSceneFile("models/bathroom");

    UpdateTitleScreen(increment * i++);
    //CompileSceneFile("models/kitchen_counter");

    UpdateTitleScreen(increment * i++);
    //CompileSceneFile("models/shadows");

    UpdateTitleScreen(increment * i++);
    //CompileSceneFile("models/fixtures");

    UpdateTitleScreen(increment * i++);
    //CompileSceneFile("models/bed");

    UpdateTitleScreen(increment * i++);
    //CompileSceneFile("models/baseboards");

    UpdateTitleScreen(increment * i++);
    //CompileSceneFile("models/test");

    UpdateTitleScreen(increment * i++);
    //CompileSceneFile("Assets/Models/tangent_test", true);

    UpdateTitleScreen(increment * i++);
    //CompileSceneFile("models/light_volumes");

    UpdateTitleScreen(increment * i++);
    //CompileSceneFile("models/occlusion_volumes");

    UpdateTitleScreen(increment * i++);
    //CompileSceneFile("models/office");

    UpdateTitleScreen(increment * i++);
   // CompileSceneFile("models/clocks");

    // EXPENSIVE
    UpdateTitleScreen(increment * i++);
    //CompileSceneFile("models/kitchen_sink");

    UpdateTitleScreen(increment * i++);
    //CompileSceneFile("models/oven");

    UpdateTitleScreen(increment * i++);
    //CompileSceneFile("models/engine");
}

void ApartmentSimulator::LoadSceneAssets() {
    float increment = 1 / 23.0f;
    int i = 1;

    m_apartmentGroup = LoadSceneFile("models/apartment");				UpdateTitleScreen(increment * i++);
    m_bathroomGroup = LoadSceneFile("models/bathroom");					UpdateTitleScreen(increment * i++);
    m_largeClosetGroup = LoadSceneFile("models/large_closet");			UpdateTitleScreen(increment * i++);
    m_fridgeGroup = LoadSceneFile("models/Fridge");						UpdateTitleScreen(increment * i++);
    m_environmentGroup = LoadSceneFile("models/environment");			UpdateTitleScreen(increment * i++);
    m_balconyGroup = LoadSceneFile("models/balcony");					UpdateTitleScreen(increment * i++);
    m_kitchenCabinetsGroup = LoadSceneFile("models/kitchen_cabinets");	UpdateTitleScreen(increment * i++);
    m_radiatorGroup = LoadSceneFile("models/radiators");				UpdateTitleScreen(increment * i++);
    m_doorFramesGroup = LoadSceneFile("models/door_frames");			UpdateTitleScreen(increment * i++);
    m_ovenGroup = LoadSceneFile("models/oven");							UpdateTitleScreen(increment * i++);
    //m_engineGroup = LoadSceneFile("Assets/Models/engine");
    m_kitchenCounterGroup = LoadSceneFile("models/kitchen_counter");	UpdateTitleScreen(increment * i++);
    m_kitchenSinkGroup = LoadSceneFile("models/kitchen_sink");			UpdateTitleScreen(increment * i++);
    m_fixturesGroup = LoadSceneFile("models/fixtures");					UpdateTitleScreen(increment * i++);
    m_testGroup = LoadSceneFile("models/test");							UpdateTitleScreen(increment * i++);
    m_bedGroup = LoadSceneFile("models/bed");							UpdateTitleScreen(increment * i++);
    m_baseboardGroup = LoadSceneFile("models/baseboards");				UpdateTitleScreen(increment * i++);
    m_tangentTestGroup = LoadSceneFile("models/tangent_test");			UpdateTitleScreen(increment * i++);
    m_officeGroup = LoadSceneFile("models/office");						UpdateTitleScreen(increment * i++);
    m_clockGroup = LoadSceneFile("models/clocks");						UpdateTitleScreen(increment * i++);

    m_occlusionVolumes = LoadSceneFile("models/occlusion_volumes");		UpdateTitleScreen(increment * i++);
    m_volumes = LoadSceneFile("models/light_volumes");					UpdateTitleScreen(increment * i++);

    LoadSceneFile("models/shadows");									UpdateTitleScreen(increment * i++);
}
