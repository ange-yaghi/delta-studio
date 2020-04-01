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
        dphysics::RigidBody *m_bone;
        dphysics::RigidBody *m_bone001;

        ysTexture *m_demoTexture;

        ysAnimationAction m_riseAction;
        ysAnimationAction m_twistAction;

        ysAnimationActionBinding m_riseBinding;
        ysAnimationActionBinding m_twistBinding;

        TransformTarget m_boneTarget;
        TransformTarget m_bone001Target;

        ysQuaternion m_boneBase;
        ysQuaternion m_bone001Base;

        ysAnimationMixer m_animationMixer;
        ysAnimationChannel *m_channel1;
        ysAnimationChannel *m_channel2;
    };

} /* namesapce dbasic_demo */

#endif /* DBASIC_DEMO_DELTA_BASIC_DEMO_APPLICATION_H */
