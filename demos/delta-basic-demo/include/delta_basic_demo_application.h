#ifndef DELTA_DEMO_DELTA_BASIC_DEMO_APPLICATION_H
#define DELTA_DEMO_DELTA_BASIC_DEMO_APPLICATION_H

#include "demo_application.h"

#include "delta.h"

namespace delta_demo {

    class DeltaBasicDemoApplication : public DemoApplication {
    public:
        DeltaBasicDemoApplication();
        virtual ~DeltaBasicDemoApplication();

        virtual void Initialize(void *instance, ysContextObject::DeviceAPI api);
        virtual void Run();
        virtual void Destroy();

    protected:
        void Process();
        void Render();

        dbasic::DeltaEngine m_engine;
        dbasic::AssetManager m_assetManager;

        dbasic::ShaderSet m_shaderSet;
        dbasic::DefaultShaders m_shaders;

        dbasic::RenderSkeleton *m_renderSkeleton;
        ysTransform m_skeletonBase;
        ysTransform *m_probe;
        dbasic::RenderNode *m_probe2;

        ysTexture *m_demoTexture;

        ysAnimationAction *m_blinkAction;
        ysAnimationAction *m_idleAction;
        ysAnimationAction *m_walkAction;

        ysAnimationActionBinding m_blink;
        ysAnimationActionBinding m_idle;
        ysAnimationActionBinding m_walk;

        ysAnimationChannel *m_channel1;
        ysAnimationChannel *m_channel2;

        float m_blinkTimer;
    };

} /* namesapce dbasic_demo */

#endif /* DELTA_DEMO_DELTA_BASIC_DEMO_APPLICATION_H */
