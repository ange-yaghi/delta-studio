#ifndef DELTA_BASIC_DELTA_ENGINE_H
#define DELTA_BASIC_DELTA_ENGINE_H

#include "delta_core.h"

#include "window_handler.h"
#include "shader_controls.h"
#include "animation.h"

#include "rigid_body_system.h"

#include "model_asset.h"
#include "mass_spring_system.h"
#include "console.h"

namespace dbasic {

    struct DrawCall {
        ysTexture *Texture;
        ShaderObjectVariables ObjectVariables;
        ModelAsset *Model;
    };

    class DeltaEngine : public ysObject {
    public:
        static const int MAX_LAYERS = 256;

        enum DRAW_TARGET {
            DRAW_TARGET_GUI,
            DRAW_TARGET_MAIN
        };

    public:
        DeltaEngine();
        ~DeltaEngine();

        // Physics Interface
        MassSpringSystem PhysicsSystem;

        ysError CreateGameWindow(const char *title, void *instance, ysContextObject::DEVICE_API API, const char *shaderDirectory = "../DeltaEngineTullahoma/Shaders/", bool depthBuffer = true);
        ysError StartFrame();
        ysError EndFrame();
        ysError Destroy();

        ysError DrawImage(ysTexture *image, int layer = 0, float scaleX = 1.0f, float scaleY = 1.0f, float texOffsetU = 0.0f, float texOffsetV = 0.0f, float texScaleX = 1.0f, float texScaleY = 1.0f);
        ysError DrawBox(const int color[3], float width, float height, int layer = 0);
        ysError DrawAxis(const int color[3], const ysVector &position, const ysVector &direction, float width, float length, int layer = 0);
        ysError DrawModel(ModelAsset *model, const ysMatrix &transform, float scale, ysTexture *texture, int layer = 0);
        ysError LoadTexture(ysTexture **image, const char *fname);
        ysError LoadAnimation(Animation **animation, const char *path, int start, int end);

        void SubmitSkeleton(Skeleton *skeleton);

        bool IsOpen() { return m_gameWindow->IsOpen(); }

        void SetWindowSize(int width, int height);

        void SetClearColor(int r, int g, int b);

        // Shader Controls
        void SetCameraPosition(float x, float y);
        void SetCameraAngle(float angle);
        void SetCameraAltitude(float altitude);

        void SetObjectTransform(const ysMatrix &mat);
        void SetPositionOffset(const ysVector &position);

        // Input Device
        bool IsKeyDown(ysKeyboard::KEY_CODE key);
        bool ProcessKeyDown(ysKeyboard::KEY_CODE key);

        bool IsMouseKeyDown(ysMouse::BUTTON_CODE key);
        int GetMouseWheel();
        void GetMousePos(int *x, int *y);

        float GetFrameLength();

        void SetMultiplyColor(ysVector4 color);
        void ResetMultiplyColor();

        ysDevice *GetDevice() { return m_device; }

        void SetDrawTarget(DRAW_TARGET target) { m_currentTarget = target; }

        int GetScreenWidth();
        int GetScreenHeight();

        Console *GetConsole() { return &m_console; }

    protected:
        float m_cameraX;
        float m_cameraY;
        float m_cameraAltitude;
        float m_cameraAngle;

        ysMatrix m_perspectiveProjection;
        ysMatrix m_orthographicProjection;

        //protected:
    public:
        ysDevice *m_device;

        ysWindowSystem *m_windowSystem;
        ysInputSystem *m_inputSystem;

        WindowHandler m_windowHandler;
        ysWindow *m_gameWindow;

        ysRenderingContext *m_renderingContext;
        ysRenderTarget *m_mainRenderTarget;

        ysAudioSystem *m_audioSystem;

        ysGPUBuffer *m_mainVertexBuffer;
        ysGPUBuffer *m_mainIndexBuffer;

        ysInputDevice *m_mainKeyboard;
        ysInputDevice *m_mainMouse;

        // Shader Controls

        ysGPUBuffer *m_shaderObjectVariablesBuffer;
        ShaderObjectVariables m_shaderObjectVariables;
        bool m_shaderObjectVariablesSync;

        ysGPUBuffer *m_shaderScreenVariablesBuffer;
        ShaderScreenVariables m_shaderScreenVariables;
        bool m_shaderScreenVariablesSync;

        ysGPUBuffer *m_shaderSkinningControlsBuffer;
        ShaderSkinningControls m_shaderSkinningControls;

        ysShader *m_vertexShader;
        ysShader *m_vertexSkinnedShader;
        ysShader *m_pixelShader;
        ysShaderProgram *m_shaderProgram;
        ysShaderProgram *m_skinnedShaderProgram;

        ysRenderGeometryFormat m_skinnedFormat;
        ysRenderGeometryFormat m_standardFormat;
        ysInputLayout *m_skinnedInputLayout;
        ysInputLayout *m_inputLayout;

        // Text Support
        Console m_console;

        bool m_initialized;

        DRAW_TARGET m_currentTarget;

        // Timing
        ysTimingSystem *m_timingSystem;

    protected:
        // Settings
        float m_clearColor[4];

    protected:
        // Initialization Routines
        ysError InitializeGeometry();
        ysError InitializeShaders(const char *shaderDirectory);
        ysError InitializeRendering();

    protected:
        // Drawing queues
        ysExpandingArray<DrawCall, 256> m_drawQueue[MAX_LAYERS];
        ysExpandingArray<DrawCall, 256> m_drawQueueGui[MAX_LAYERS];
        ysError ExecuteDrawQueue(DRAW_TARGET target);
    };

} /* namesapce dbasic */

#endif /* DELTA_BASIC_DELTA_ENGINE_H */
