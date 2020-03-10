#ifndef DELTA_BASIC_COLLISION_OBJECT_H
#define DELTA_BASIC_COLLISION_OBJECT_H

#include "delta_core.h"

#include "collision_primitives.h"

namespace dphysics {

    class RigidBody;
    class CollisionObject : public ysObject {
    public:
        enum class Mode {
            Sensor = 0x1,

            Fine = 0x2,
            Coarse = 0x4,

            Undefined,
        };

        enum class Type {
            Box,
            Circle,

            Undefined
        };

    public:
        CollisionObject();
        CollisionObject(Type type);
        virtual ~CollisionObject();

        inline Type GetType() const { return m_type; }

        BoxPrimitive *GetAsBox() { return static_cast<BoxPrimitive *>(m_primitiveHandle); }
        CirclePrimitive *GetAsCircle() { return static_cast<CirclePrimitive *>(m_primitiveHandle); }

        Mode GetMode() const { return m_mode; }
        void SetMode(Mode mode) { m_mode = mode; }

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
        Type m_type;
        Mode m_mode;

        unsigned int m_msg;

        RigidBody *m_parent;
    };

    template <typename T_Type, CollisionObject::Type typeID>
    class CollisionObjectSpecialized : public CollisionObject {
    public:
        CollisionObjectSpecialized() : CollisionObject(typeID) {
            m_primitiveHandle = (void *)&m_collisionPrimitive;
        }

        virtual ~CollisionObjectSpecialized() {
            /* void */
        }

    protected:
        T_Type m_collisionPrimitive;
    };

} /* namespace dbasic */

#endif /* DELTA_BASIC_COLLISION_OBJECT_H */
