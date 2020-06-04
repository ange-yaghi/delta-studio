#ifndef DELTA_BASIC_DELTA_ENGINE_H
#define DELTA_BASIC_DELTA_ENGINE_H

#include "delta_core.h"

#include "window_handler.h"
#include "shader_controls.h"
#include "animation.h"

#include "../../../physics/include/mass_spring_system.h"
#include "../../../physics/include/rigid_body_system.h"

#include "model_asset.h"
#include "audio_asset.h"
#include "console.h"

namespace dbasic {

    class RenderSkeleton;

    class DeltaEngine : public ysObject {
    public:
        static const int MAX_LAYERS = 256;

        enum class DrawTarget {
            Gui,
            Main
        };

        enum class CameraMode {
            Target,
            Flat
        };

        struct DrawCall {
            ysTexture *Texture = nullptr;
            ShaderObjectVariables ObjectVariables;
            ModelAsset *Model = nullptr;
        };

    public:
        DeltaEngine();
        ~DeltaEngine();

        // Physics Interface
        dphysics::MassSpringSystem PhysicsSystem;
        dphysics::RigidBodySystem RBSystem;

        ysError CreateGameWindow(const char *title, void *instance, ysContextObject::DEVICE_API API, const char *shaderDirectory = "../DeltaEngineTullahoma/Shaders/", bool depthBuffer = true);
        ysError StartFrame();
        ysError EndFrame();
        ysError Destroy();

        ysError UseMaterial(Material *material);

        ysError AddLight(const Light &light);
        void ResetLights();
        ysError SetAmbientLight(const ysVector4 &ambient);

        ysError DrawImage(ysTexture *image, int layer = 0, float scaleX = 1.0f, float scaleY = 1.0f, float texOffsetU = 0.0f, float texOffsetV = 0.0f, float texScaleX = 1.0f, float texScaleY = 1.0f);
        ysError DrawBox(float width, float height, int layer = 0, bool lit = true);
        ysError DrawAxis(const ysVector &position, const ysVector &direction, float width, float length, int layer = 0, bool lit = false);
        ysError DrawModel(ModelAsset *model, float scale, ysTexture *texture, int layer = 0, bool lit = true);
        ysError DrawRenderSkeleton(RenderSkeleton *skeleton, float scale, int layer);
        ysError LoadTexture(ysTexture **image, const char *fname);
        ysError LoadAnimation(Animation **animation, const char *path, int start, int end);

        ysError PlayAudio(AudioAsset *audio);

        void SubmitSkeleton(Skeleton *skeleton);

        bool IsOpen() { return m_gameWindow->IsOpen(); }

        void SetWindowSize(int width, int height);

        void SetClearColor(int r, int g, int b);

        // Shader Controls
        void SetCameraPosition(float x, float y);
        void SetCameraPosition(const ysVector &pos);
        ysVector GetCameraPosition() const;
        void GetCameraPosition(float *x, float *y) const;

        void SetCameraUp(const ysVector &up);
        ysVector GetCameraUp() const;

        void SetCameraTarget(const ysVector &target);
        ysVector GetCameraTarget() const { return m_cameraTarget; }

        void SetCameraMode(CameraMode mode);
        CameraMode GetCameraMode() const { return m_cameraMode; }

        void SetCameraAngle(float angle);

        float GetCameraFov() const;
        void SetCameraFov(float fov);

        float GetCameraAspect() const;

        void SetCameraAltitude(float altitude);
        float GetCameraAltitude() const;

        void SetObjectTransform(const ysMatrix &mat);
        void SetPositionOffset(const ysVector &position);

        // Input Device
        bool IsKeyDown(ysKeyboard::KEY_CODE key);
        bool ProcessKeyDown(ysKeyboard::KEY_CODE key);

        bool IsMouseKeyDown(ysMouse::BUTTON_CODE key);
        int GetMouseWheel();
        void GetMousePos(int *x, int *y);

        float GetFrameLength();
        float GetAverageFramerate();

        void ResetBrdfParameters();
        void SetBaseColor(const ysVector &color);
        void ResetBaseColor();

        void SetEmission(const ysVector &emission);
        void SetSpecularMix(float specularMix);
        void SetDiffuseMix(float diffuseMix);
        void SetMetallic(float metallic);
        void SetDiffuseRoughness(float diffuseRoughness);
        void SetSpecularRoughness(float specularRoughness);
        void SetSpecularPower(float power);
        void SetIncidentSpecular(float incidentSpecular);

        ysDevice *GetDevice() { return m_device; }

        void SetDrawTarget(DrawTarget target) { m_currentTarget = target; }

        int GetScreenWidth();
        int GetScreenHeight();

        Console *GetConsole() { return &m_console; }

        ysAudioDevice *GetAudioDevice() const { return m_audioDevice; }

    protected:
        float m_cameraAngle;
        float m_cameraFov;

        ysVector m_cameraPosition;
        ysVector m_cameraTarget;
        ysVector m_cameraUp;

        ysMatrix m_perspectiveProjection;
        ysMatrix m_orthographicProjection;

    protected:
        ysDevice *m_device;

        ysWindowSystem *m_windowSystem;
        ysInputSystem *m_inputSystem;

        WindowHandler m_windowHandler;
        ysWindow *m_gameWindow;

        ysRenderingContext *m_renderingContext;
        ysRenderTarget *m_mainRenderTarget;

        ysAudioSystem *m_audioSystem;
        ysAudioDevice *m_audioDevice;

        ysGPUBuffer *m_mainVertexBuffer;
        ysGPUBuffer *m_mainIndexBuffer;

        ysInputDevice *m_mainKeyboard;
        ysInputDevice *m_mainMouse;

        // Shader Controls
        ysGPUBuffer *m_shaderObjectVariablesBuffer;
        ShaderObjectVariables m_shaderObjectVariables;
        bool m_shaderObjectVariablesSync;

        ysGPUBuffer *m_consoleShaderObjectVariablesBuffer;
        ConsoleShaderObjectVariables m_consoleShaderObjectVariables;

        ysGPUBuffer *m_lightingControlBuffer;
        LightingControls m_lightingControls;
        int m_lightCount;

        ysGPUBuffer *m_shaderScreenVariablesBuffer;
        ShaderScreenVariables m_shaderScreenVariables;
        bool m_shaderScreenVariablesSync;

        ysGPUBuffer *m_shaderSkinningControlsBuffer;
        ShaderSkinningControls m_shaderSkinningControls;

        ysShader *m_vertexShader;
        ysShader *m_vertexSkinnedShader;
        ysShader *m_pixelShader;
        ysShader *m_consoleVertexShader;
        ysShader *m_consolePixelShader;
        ysShaderProgram *m_shaderProgram;
        ysShaderProgram *m_skinnedShaderProgram;
        ysShaderProgram *m_consoleProgram;

        ysRenderGeometryFormat m_skinnedFormat;
        ysRenderGeometryFormat m_standardFormat;
        ysRenderGeometryFormat m_consoleVertexFormat;
        ysInputLayout *m_skinnedInputLayout;
        ysInputLayout *m_inputLayout;
        ysInputLayout *m_consoleInputLayout;

        // Text Support
        Console m_console;

        bool m_initialized;

        DrawTarget m_currentTarget;
        CameraMode m_cameraMode;

        // Timing
        ysTimingSystem *m_timingSystem;

    protected:
        // Settings
        float m_clearColor[4];

    protected:
        // Initialization Routines
        ysError InitializeGeometry();
        ysError InitializeShaders(const char *shaderDirectory);

    protected:
        // Drawing queues
        ysExpandingArray<DrawCall, 256> m_drawQueue[MAX_LAYERS];
        ysExpandingArray<DrawCall, 256> m_drawQueueGui[MAX_LAYERS];
        ysError ExecuteDrawQueue(DrawTarget target);
    };

} /* namesapce dbasic */

#endif /* DELTA_BASIC_DELTA_ENGINE_H */
