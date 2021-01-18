#ifndef DELTA_BASIC_DEFAULT_SHADERS_H
#define DELTA_BASIC_DEFAULT_SHADERS_H

#include "delta_core.h"

#include "shader_controls.h"
#include "shader_set.h"

namespace dbasic {

    class DefaultShaders : public ysObject {
    public:
        DefaultShaders();
        virtual ~DefaultShaders();

        ysError Initialize(ysDevice *device, ysRenderTarget *renderTarget, ysShaderProgram *shaderProgram, ysInputLayout *inputLayout);
        ysError Destroy();

        ShaderSet *GetShaderSet() { return &m_shaderSet; }

        void ResetBrdfParameters();
        void SetBaseColor(const ysVector &color);
        void ResetBaseColor();

        void SetScale(float x = 1.0f, float y = 1.0f, float z = 1.0f);
        void SetTexOffset(float u, float v);
        void SetTexScale(float u, float v);

        void SetColorReplace(bool colorReplace);
        void SetLit(bool lit);
        void SetEmission(const ysVector &emission);
        void SetSpecularMix(float specularMix);
        void SetDiffuseMix(float diffuseMix);
        void SetMetallic(float metallic);
        void SetDiffuseRoughness(float diffuseRoughness);
        void SetSpecularRoughness(float specularRoughness);
        void SetSpecularPower(float power);
        void SetIncidentSpecular(float incidentSpecular);
        void SetFogNear(float fogNear);
        void SetFogFar(float fogFar);
        void SetFogColor(const ysVector &color);

        void SetObjectTransform(const ysMatrix &mat);
        void SetPositionOffset(const ysVector &position);

        void SetProjection(const ysMatrix &mat);
        void SetCameraView(const ysMatrix &mat);
        void SetEye(const ysVector &vec);

        ysError AddLight(const Light &light);
        void ResetLights();
        ysError SetAmbientLight(const ysVector4 &ambient);

    protected:
        ShaderScreenVariables m_shaderScreenVariables;
        ShaderObjectVariables m_shaderObjectVariables;
        LightingControls m_lightingControls;
        int m_lightCount;

    protected:
        ShaderSet m_shaderSet;
    };

} /* namespace dbasic */

#endif /* DELTA_BASIC_DEFAULT_SHADERS_H */
