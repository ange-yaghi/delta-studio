#ifndef APTSIM_MATERIAL_H
#define APTSIM_MATERIAL_H

#include "../../../include/yds_texture.h"

class Material : public ysObject {
public:
    Material() : ysObject("Material") {
        m_name[0] = '\0';

        m_specular = 1.0;
        m_specularPower = 1.0;
        m_ambient = 1.0;

        m_falloffPower = 1.0;
        m_useFalloff = 0;
        m_falloffColor = ysVector4(1.0, 1.0, 1.0, 1.0);

        m_diffuseColor = ysVector4(1.0, 1.0, 1.0, 1.0);

        m_useAlpha = 0;
        m_reflectPower = 1.0f;

        m_diffuseMap = NULL;
        m_ambientMap = NULL;
        m_specularMap = NULL;
        m_normalMap = NULL;
        m_reflectMap = NULL;

        m_lit = 1;
    }

    ~Material() {
        /* void */
    }

    char m_name[64];

    ysVector4 m_diffuseColor;

    ysVector4 m_falloffColor;
    float m_falloffPower;
    int m_useFalloff;
    int m_useAlpha;

    float m_specular;
    float m_specularPower;
    float m_ambient;
    float m_reflectPower;

    ysTexture *m_diffuseMap;
    ysTexture *m_ambientMap;
    ysTexture *m_specularMap;
    ysTexture *m_normalMap;
    ysTexture *m_reflectMap;

    int m_lit;
};

#endif /* APTSIM_MATERIAL_H */
