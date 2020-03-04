#ifndef DELTA_BASIC_COLLISION_OBJECT_H
#define DELTA_BASIC_COLLISION_OBJECT_H

#include "delta_core.h"

#include "collision_primitives.h"

namespace dbasic {

    class RigidBody;
    class CollisionObject : public ysObject {
    public:
        enum COLLISION_OBJECT_MODE {
            COLLISION_OBJECT_MODE_SENSOR = 0x1,

            COLLISION_OBJECT_MODE_COLLISION_FINE = 0x2,
            COLLISION_OBJECT_MODE_COLLISION_COARSE = 0x4,

            COLLISION_OBJECT_MODE_UNDEFINED,
        };

        enum COLLISION_OBJECT_TYPE {
            COLLISION_OBJECT_TYPE_BOX,
            COLLISION_OBJECT_TYPE_CIRCLE,

            COLLISION_OBJECT_TYPE_UNDEFINED
        };

    public:
        CollisionObject();
        CollisionObject(COLLISION_OBJECT_TYPE type);
        virtual ~CollisionObject();

        inline COLLISION_OBJECT_TYPE GetType() const { return m_type; }

        BoxPrimitive *GetAsBox() { return static_cast<BoxPrimitive *>(m_primitiveHandle); }
        CirclePrimitive *GetAsCircle() { return static_cast<CirclePrimitive *>(m_primitiveHandle); }

        COLLISION_OBJECT_MODE GetMode() const { return m_mode; }
        void SetMode(COLLISION_OBJECT_MODE mode) { m_mode = mode; }

        void SetParent(RigidBody *parent) { m_parent = parent; }
        RigidBody *GetParent() const { return m_parent; }

        void SetRelativePosition(const ysVector &pos) { m_relativePosition = pos; }
        void SetRelativeOrientation(const ysMatrix &orientation) { m_relativeOrientation = orientation; }

        ysVector GetRelativePosition() const { return m_relativePosition; }
        ysMatrix GetRelativeOrientation() const { return m_relativeOrientation; }

        void ConfigurePrimitive();

        void SetCollisionLayerExclusionMask(unsigned int mask) { m_collisionLayerMask = ~mask; }
        unsigned int GetCollisionLasyerMask() const { return m_collisionLayerMask; }

        void SetLayerMask(unsigned int mask) { m_layerMask = mask; }
        unsigned int GetLasyerMask() const { return m_layerMask; }

        bool CheckCollisionMask(const CollisionObject *object) const;

        void SetEventMessage(unsigned int msg) { m_msg = msg; }
        unsigned int GetEventMessage() const { return m_msg; }

    protected:
        void ConfigureBox();
        void ConfigureCircle();

        void *m_primitiveHandle;

        ysVector m_relativePosition;
        ysMatrix m_relativeOrientation;

        unsigned int m_layerMask;
        unsigned int m_collisionLayerMask;

    private:
        COLLISION_OBJECT_TYPE m_type;
        COLLISION_OBJECT_MODE m_mode;

        unsigned int m_msg;

        RigidBody *m_parent;
    };

    template <typename Type, CollisionObject::COLLISION_OBJECT_TYPE typeID>
    class CollisionObjectSpecialized : public CollisionObject {
    public:
        CollisionObjectSpecialized() : CollisionObject(typeID) {
            m_primitiveHandle = (void *)&m_collisionPrimitive;
        }

        virtual ~CollisionObjectSpecialized() {
            /* void */
        }

    protected:
        Type m_collisionPrimitive;
    };

} /* namespace dbasic */

#endif /* DELTA_BASIC_COLLISION_OBJECT_H */
