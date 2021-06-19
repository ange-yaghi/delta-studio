#include "../include/material.h"

#include <string.h>

dbasic::Material::Material() : ysObject("Material") {
    m_name[0] = '\0';

    m_diffuseMap = nullptr;
    m_normalMap = nullptr;
    m_specularMap = nullptr;
    m_aoMap = nullptr;

    m_lit = true;

    m_specularMix = 1.0f;
    m_diffuseMix = 1.0f;
    m_metallic = 0.0f;
    m_diffuseRoughness = 0.5f;
    m_specularPower = 4.0f;
    m_incidentSpecular = 1.0f;

    m_diffuseColor = ysVector4(1.0f, 1.0f, 1.0f, 1.0f);
}

dbasic::Material::~Material() {
    /* void */
}

void dbasic::Material::SetName(const char *name) {
    strcpy_s(m_name, 64, name);
}

void dbasic::Material::SetDiffuseColor(int r, int g, int b, int a) {
    m_diffuseColor = ysVector4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}

void dbasic::Material::SetDiffuseColor(const ysVector &diffuse) {
    m_diffuseColor = ysMath::GetVector4(diffuse);
}
