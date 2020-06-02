#ifndef DELTA_BASIC_MATERIAL_H
#define DELTA_BASIC_MATERIAL_H

#include "delta_core.h"

namespace dbasic {

    class Material : public ysObject {
    public:
        Material();
        ~Material();

        void SetName(const char *name);
        const char *GetName() { return m_name; }

        void SetDiffuseColor(int r, int g, int b, int a = 255);
        void SetDiffuseColor(const ysVector &diffuse);
        ysVector GetDiffuseColor() const { return ysMath::LoadVector(m_diffuseColor); }

        void SetDiffuseMap(ysTexture *diffuseMap) { m_diffuseMap = diffuseMap; }
        ysTexture *GetDiffuseMap() { return m_diffuseMap; }

        void SetSpecularMap(ysTexture *specularMap) { m_specularMap = specularMap; }
        ysTexture *GetSpecularMap() { return m_specularMap; }

        void SetNormalMap(ysTexture *normalMap) { m_normalMap = normalMap; }
        ysTexture *GetNormalMap() { return m_normalMap; }

        bool UsesNormalMap() const { return m_normalMap != nullptr; }
        bool UsesSpecularMap() const { return m_specularMap != nullptr; }
        bool UsesDiffuseMap() const { return m_diffuseMap != nullptr; }

    protected:
        char m_name[64];

        ysVector4 m_diffuseColor;

        ysTexture *m_diffuseMap;
        ysTexture *m_specularMap;
        ysTexture *m_normalMap;
    };

} /* namespace dbasic */

#endif /* DELTA_BASIC_MATERIAL_H */
