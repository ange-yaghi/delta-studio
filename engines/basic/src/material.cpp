#include "../include/material.h"

#include <string.h>

dbasic::Material::Material() : ysObject("Material") {
    /* void */
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

void dbasic::Material::SetDiffuseColor(ysVector4 diffuse) {
    m_diffuseColor = diffuse;
}
