#ifndef APTSIM_LIGHTING_H
#define APTSIM_LIGHTING_H

#include "SceneObject.h"

#include "../../../include/yds_math.h"

#define NUM_LIGHTS 16
#define NUM_SHADOWS 16

struct Light {
    ysVector4 Location;
    ysVector4 Color;
    ysVector4 LowColor;
    ysVector4 Direction;
    ysVector4 AuxCutoffDirection;
    float AuxCutoffAmount;

    float CutoffHigh;
    float CutoffLow;
    float CutoffPower;
    float CutoffMix;
    int CutoffAmbient;

    float Falloff;
    float Intensity;

    float Ambient;
    float Diffuse;

    float Specular;
    float SpecularPower;

    //int PAD[1];
};

struct PlaneShadow {
    ysVector4 Location;
    ysVector4 FringeMask;
    float ShadowIntensity;
    float HalfWidth;
    float HalfDepth;
    float HalfHeight;
    float Falloff;

    int PAD[3];
};

class LightObject : public ysObject {
public:
    LightObject();
    ~LightObject();

    void SetName(const char *name) { strcpy_s(m_name, 64, name); }

    LightVolume *m_volume;

    ysVector4 m_location;
    ysVector4 m_color;
    ysVector4 m_lowColor;
    ysVector4 m_direction;
    ysVector4 m_auxCutoffDirection;

    float m_auxCutoffAmount;

    float m_cutoffHigh;
    float m_cutoffLow;
    float m_cutoffPower;
    float m_cutoffMix;
    int m_cutoffAmbient;

    float m_falloff;
    float m_intensity;

    float m_ambient;
    float m_diffuse;

    float m_specular;
    float m_specularPower;

    LightObject *m_reference;

    bool m_virtual;
    bool m_on;

    char m_name[64];
};

class ShadowObject : public ysObject {
public:
    ShadowObject();
    ~ShadowObject();

    LightVolume *m_volume;

    void SetName(const char *name) { strcpy_s(m_name, 64, name); }

    ysVector4 m_location;
    ysVector4 m_fringeMask;
    float m_shadowIntensity;
    float m_width;
    float m_depth;
    float m_height;
    float m_falloff;

    char m_name[64];
};

struct LightsList {
    int NumLights;
    int NumShadows;

    int PAD[2];

    Light Lights[NUM_LIGHTS];
    PlaneShadow Shadows[NUM_SHADOWS];
};

#endif /* APTSIM_LIGHTING_H */
