#ifndef DELTA_BASIC_RIGID_BODY_H
#define DELTA_BASIC_RIGID_BODY_H

#include "delta_core.h"

#include "collision_geometry.h"

namespace dphysics {

    class Collision;
    class RigidBodySystem;
    class ForceGenerator;

    class RigidBody : public ysObject {
        friend RigidBodySystem;

    public:
        enum class RigidBodyHint {
            Static,
            Dynamic
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
        void UpdateDerivedData(bool force = false);
        void CheckAwake();

        ysQuaternion GetOrientation() const { return m_orientation; }

        void SetOrientation(const ysQuaternion &q) { m_orientation = q; m_derivedValid = false; }
        ysVector GetPosition() const { return m_position; }
        ysVector GetWorldPosition() const { return m_worldPosition; }
        ysQuaternion GetWorldOrientation() const { return m_finalOrientation; }

        void AddAngularVelocity(const ysVector &v) { m_angularVelocity = ysMath::Add(v, m_angularVelocity); }

        void SetPosition(const ysVector &v) { m_position = v; m_derivedValid = false; }
        void SetVelocity(const ysVector &v) { m_velocity = v; }
        ysVector GetVelocity() const { return m_velocity; }

        void SetAngularVelocity(const ysVector &v) { m_angularVelocity = v; }
        ysVector GetAngularVelocity() const { return m_angularVelocity; }

        ysMatrix GetTransform() const { return m_transform; }
        ysMatrix GetInverseTransform() const { return m_inverseTransform; }
        ysMatrix GetOrientationMatrix() const { return m_orientationOnly; }

        ysVector GetVelocityLocal(const ysVector &p) const;

        ysVector GetWorldOrientation(const ysVector &v) const { return ysMath::MatMult(m_orientationOnly, v); }
        ysVector GetGlobalSpace(const ysVector &v) const { return ysMath::MatMult(m_transform, v); }
        ysVector GetLocalSpace(const ysVector &v) const { return ysMath::MatMult(m_inverseTransform, v); }

        ysMatrix GetInverseInertiaTensor() const { return m_inverseInertiaTensor; }
        ysMatrix GetInverseInertiaTensorWorld() const { return m_inverseInertiaTensorWorld; }

        void SetInverseInertiaTensor(const ysMatrix &tensor);
        ysMatrix GetRectangleTensor(float dx, float dy);

        void SetInverseMass(float inverseMass) { m_inverseMass = inverseMass; }

        float GetInverseMass() const { return m_inverseMass; }

        RigidBody *GetRoot() { if (m_parent != nullptr) return m_parent->GetRoot(); else return this; }
        void AddChild(RigidBody *body);
        void RemoveChild(RigidBody *child);
        void ClearChildren() { m_children.Clear(); }

        void RequestCollisions();
        void ClearCollisions() { m_collisions.Clear(); }
        void AddCollision(Collision *collision) { SetAwake(true); m_collisions.New() = collision; }
        int GetCollisionCount() { return m_collisions.GetNumObjects(); }
        Collision *GetCollision(int index) { return m_collisions[index]; }

        void *GetOwner() { return m_owner; }
        void SetOwner(void *owner) { m_owner = owner; }

        bool IsRegistered() const { return m_registered; }

        bool IsAwake() const { return m_awake; }
        void SetAwake(bool awake) { m_awake = awake; }

        bool RequestsInformation() const { return m_requestsInformation; }
        void SetRequestsInformation(bool ri) { m_requestsInformation = ri; }

        bool IsAlwaysAwake() const { return m_alwaysAwake; }
        void SetAlwaysAwake(bool alwaysAwake) { m_alwaysAwake = alwaysAwake; }

        void SetHint(RigidBodyHint hint) { m_hint = hint; }
        RigidBodyHint GetHint() const { return m_hint; }

        void AddGridCell(int x, int y);
        void ClearGridCells() { m_gridCells.Clear(); }
        int GetGridCellCount() { return m_gridCells.GetNumObjects(); }
        GridCell *GetGridCells() { return m_gridCells.GetBuffer(); }

        void SetAcceleration(ysVector &acceleration) { m_acceleration = acceleration; }
        ysVector GetAcceleration() const { return m_acceleration; }

        void AddAngularImpulseLocal(const ysVector &impulse);
        void AddImpulseLocalSpace(const ysVector &impulse, const ysVector &localPoint);
        void AddImpulseWorldSpace(const ysVector &impulse, const ysVector &point);
        void ClearAngularImpulseAccumulator() { m_angularImpulseAccum = ysMath::Constants::Zero; }
        void ClearImpulseAccumulator() { m_impulseAccum = ysMath::Constants::Zero; }

        void AddForceLocalSpace(const ysVector &force, const ysVector &localPoint);
        void AddForceWorldSpace(const ysVector &force, const ysVector &point);
        void ClearForceAccumulator() { m_forceAccum = ysMath::Constants::Zero; }
        ysVector GetForce() const { return m_forceAccum; }

        void AddTorque(const ysVector &torque) { m_torqueAccum = ysMath::Add(m_torqueAccum, torque); }
        void AddTorqueLocal(const ysVector &torque);
        void ClearTorqueAccumulator() { m_torqueAccum = ysMath::Constants::Zero; }
        ysVector GetTorque() const { return m_torqueAccum; }

        void ClearAccumulators();

        void SetGhost(bool ghost) { m_ghost = ghost; }
        bool IsGhost() const { return m_ghost; }

        void SetLinearDamping(float damping) { m_linearDamping = damping; }

        template <typename T_ForceGenerator>
        T_ForceGenerator *NewForceGenerator() {
            T_ForceGenerator *newForceGenerator = 
                m_forceGenerators.NewGeneric<T_ForceGenerator>();
            newForceGenerator->Initialize(this);

            return newForceGenerator;
        }

        void GenerateForces(float dt);

    protected:
        // Properties
        bool m_registered;
        bool m_awake;
        bool m_requestsInformation;
        bool m_alwaysAwake;

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
        ysVector m_impulseAccum;
        ysVector m_angularImpulseAccum;

        ysMatrix m_inverseInertiaTensor;

        // Derived
        bool m_derivedValid;
        bool m_ghost;

        ysVector m_worldPosition;
        ysVector m_lastWorldPosition;
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
        ysDynamicArray<ForceGenerator, 4> m_forceGenerators;

        RigidBodyHint m_hint;

        void *m_owner;
    };

} /* namespace dbasic */

#endif /* DELTA_BASIC_RIGID_BODY_H */
