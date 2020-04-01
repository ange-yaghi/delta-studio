#ifndef DBASIC_DEMO_DELTA_BASIC_DEMO_APPLICATION_H
#define DBASIC_DEMO_DELTA_BASIC_DEMO_APPLICATION_H

#include "delta_basic.h"

namespace dbasic_demo {

    class DeltaBasicDemoApplication {
    public:
        DeltaBasicDemoApplication();
        ~DeltaBasicDemoApplication();

        void Initialize(void *instance, ysContextObject::DEVICE_API api);
        void Run();

        float m_currentAngle;

    protected:
        void Process();
        void Render();

        dbasic::DeltaEngine m_engine;
        dbasic::AssetManager m_assetManager;

        dbasic::RenderSkeleton *m_renderSkeleton;
        dphysics::RigidBody m_skeletonBase;

        ysTexture *m_demoTexture;

        ysAnimationAction *m_riseAction;
        ysAnimationAction *m_twistAction;

        ysAnimationActionBinding m_riseBinding;
        ysAnimationActionBinding m_twistBinding;

        ysAnimationChannel *m_channel1;
        ysAnimationChannel *m_channel2;
    };

} /* namesapce dbasic_demo */

#endif /* DBASIC_DEMO_DELTA_BASIC_DEMO_APPLICATION_H */
