#ifndef DELTA_BASIC_RIGID_BODY_H
#define DELTA_BASIC_RIGID_BODY_H

#include "delta_core.h"

#include "collision_geometry.h"

namespace dbasic {

    class Collision;
    class RigidBodySystem;

    class RigidBody : public ysObject {
        friend RigidBodySystem;

    public:
        enum RIGID_BODY_HINT {
            HINT_STATIC,
            HINT_DYNAMIC,
        };

        struct GridCell {
            int x;
            int y;
        };

    public:
        RigidBody();
        ~RigidBody();

        // Interfaces
        CollisionGeometry CollisionGeometry;

        void Integrate(float timeStep);
        void UpdateDerivedData();

        ysQuaternion GetOrientation() const { return m_orientation; }

        void SetOrientation(const ysQuaternion &q) { m_orientation = q; m_derivedValid = false; }
        ysVector GetPosition() const { return m_position; }
        ysVector GetWorldPosition() const { return m_worldPosition; }
        ysQuaternion GetWorldOrientation() const { return m_finalOrientation; }

        void AddAngularVelocity(const ysVector &v) { m_angularVelocity = ysMath::Add(v, m_angularVelocity); }

        void SetPosition(const ysVector &v) { m_position = v; m_derivedValid = false; }
        void SetVelocity(const ysVector &v) { m_velocity = v; }
        void SetAngularVelocity(const ysVector &v) { m_angularVelocity = v; }

        ysMatrix GetTransform() const { return m_transform; }
        ysMatrix GetInverseTransform() const { return m_inverseTransform; }
        ysMatrix GetOrientationMatrix() const { return m_orientationOnly; }

        ysVector GetWorldOrientation(const ysVector &v) const { return ysMath::MatMult(m_orientationOnly, v); }
        ysVector GetGlobalSpace(const ysVector &v) const { return ysMath::MatMult(m_transform, v); }
        ysVector GetLocalSpace(const ysVector &v) const { return ysMath::MatMult(m_inverseTransform, v); }

        ysMatrix GetInverseInertiaTensor() const { return m_inverseInertiaTensor; }
        ysMatrix GetInverseInertiaTensorWorld() const { return m_inverseInertiaTensorWorld; }

        void SetInverseInertiaTensor(const ysMatrix &tensor);
        ysMatrix GetRectangleTensor(float dx, float dy);

        void SetInverseMass(float inverseMass) { m_inverseMass = inverseMass; }

        float GetInverseMass() const { return m_inverseMass; }

        RigidBody *GetRoot() { if (m_parent != NULL) return m_parent->GetRoot(); else return this; }
        void AddChild(RigidBody *body);
        void RemoveChild(RigidBody *child);
        void ClearChildren() { m_children.Clear(); }

        void RequestCollisions();
        void ClearCollisions() { m_collisions.Clear(); }
        void AddCollision(Collision *collision) { m_collisions.New() = collision; }
        int GetCollisionCount() { return m_collisions.GetNumObjects(); }
        Collision *GetCollision(int index) { return m_collisions[index]; }

        void *GetOwner() { return m_owner; }
        void SetOwner(void *owner) { m_owner = owner; }

        bool IsRegistered() const { return m_registered; }

        void SetHint(RIGID_BODY_HINT hint) { m_hint = hint; }
        RIGID_BODY_HINT GetHint() const { return m_hint; }

        void AddGridCell(int x, int y);
        void ClearGridCells() { m_gridCells.Clear(); }
        int GetGridCellCount() { return m_gridCells.GetNumObjects(); }
        GridCell *GetGridCells() { return m_gridCells.GetBuffer(); }

        void SetAcceleration(ysVector &acceleration) { m_acceleration = acceleration; }
        ysVector GetAcceleration() const { return m_acceleration; }

        void AddForceLocalSpace(ysVector &force, ysVector &localPoint);
        void AddForceWorldSpace(ysVector &force, ysVector &point);
        void ClearForceAccumulator() { m_forceAccum = ysMath::Constants::Zero; }
        ysVector GetForce() const { return m_forceAccum; }

        void AddTorque(const ysVector &torque) { m_torqueAccum = ysMath::Add(m_torqueAccum, torque); }
        void ClearTorqueAccumulator() { m_torqueAccum = ysMath::Constants::Zero; }
        ysVector GetTorque() const { return m_torqueAccum; }

        void ClearAccumulators() { ClearForceAccumulator(); ClearTorqueAccumulator(); }

    protected:
        // Properties
        bool m_registered;

        float m_inverseMass;
        float m_linearDamping;
        float m_angularDamping;
        ysVector m_position;
        ysQuaternion m_orientation;

        ysVector m_torqueAccum;
        ysVector m_forceAccum;
        ysVector m_acceleration;
        ysVector m_velocity;
        ysVector m_angularVelocity;

        ysMatrix m_inverseInertiaTensor;

        // Derived
        bool m_derivedValid;

        ysVector m_worldPosition;
        ysMatrix m_transform;
        ysMatrix m_inverseTransform;
        ysMatrix m_orientationOnly;
        ysMatrix m_inverseInertiaTensorWorld;
        ysQuaternion m_finalOrientation;

        ysExpandingArray<RigidBody *, 4> m_children;
        RigidBody *m_parent;
        RigidBodySystem *m_system;

        ysExpandingArray<Collision *, 4> m_collisions;

        ysExpandingArray<GridCell, 4> m_gridCells;

        RIGID_BODY_HINT m_hint;

        void *m_owner;
    };

} /* namespace dbasic */

#endif /* DELTA_BASIC_RIGID_BODY_H */
