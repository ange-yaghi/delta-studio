#ifndef DELTA_BASIC_HINGE_LINK_H
#define DELTA_BASIC_HINGE_LINK_H

#include "rigid_body_link.h"

namespace dbasic {

    class HingeLink : public RigidBodyLink {
    public:
        HingeLink();
        ~HingeLink();

        virtual int GenerateCollisions(Collision *collisionArray);
        virtual void DrawDebug(DeltaEngine *engine, int layer);

        void SetConnectionPoints(ysVector &p1, ysVector &p2) {
            m_relativePos1 = p1;
            m_relativePos2 = p2;
        }

        ysVector GetConnection1() const { return m_relativePos1; }
        ysVector GetConnection2() const { return m_relativePos2; }

    protected:
        ysVector m_relativePos1;
        ysVector m_relativePos2;
    };

} /* namespace dbasic */

#endif /* DELTA_BASIC_HINGE_LINK_H */
