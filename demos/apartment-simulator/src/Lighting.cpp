#include "../include/Lighting.h"

LightObject::LightObject() : ysObject("LIGHT") {
    m_volume = nullptr;
    m_intensity = 0.0f;
    m_cutoffAmbient = 0;

    m_auxCutoffAmount = 0.0f;
    m_reference = nullptr;

    m_virtual = false;
    m_on = true;

    SetName("");
}

LightObject::~LightObject() {
    /* void */
}

ShadowObject::ShadowObject() : ysObject("SHADOW") {
    m_shadowIntensity = 0.0f;
    m_height = 0.0f;
    m_width = 0.0f;
    m_depth = 0.0f;
    m_fringeMask = ysVector4(1.0f, 1.0f, 1.0f);
    m_volume = NULL;

    SetName("");
}

ShadowObject::~ShadowObject() {
    /* void */
}
