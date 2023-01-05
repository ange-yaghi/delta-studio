#include "../include/rigid_body.h"

#include "../include/rigid_body_system.h"
#include "../include/force_generator.h"

dphysics::RigidBody::RigidBody() {
    m_linearDamping = 0.99f;
    m_angularDamping = 0.5f;

    m_velocity = ysMath::Constants::Zero;
    m_angularVelocity = ysMath::Constants::Zero;

    m_inverseInertiaTensor = ysMath::LoadMatrix(ysMath::Constants::Zero, ysMath::Constants::Zero, ysMath::Constants::Zero, ysMath::Constants::IdentityRow4);

    m_collisionGeometry.SetParent(this);

    m_parent = nullptr;

    m_derivedValid = false;
    m_registered = false;
    m_ghost = false;

    m_owner = nullptr;
    m_hint = RigidBodyHint::Static;

    m_awake = true;
    m_requestsInformation = false;

    ClearAccumulators();
    m_acceleration = ysMath::Constants::Zero;

    m_material = -1;
}

dphysics::RigidBody::~RigidBody() {
    /* void */
}

void dphysics::RigidBody::Integrate(float timeStep) {
    if (m_hint == RigidBodyHint::Static) return;
    else m_derivedValid = false;

    ysVector vTimeStep = ysMath::LoadScalar(timeStep);

    ysQuaternion orientation = Transform.GetOrientationParentSpace();
    ysVector position = Transform.GetPositionParentSpace();
    orientation = ysMath::QuatAddScaled(orientation, m_angularVelocity, timeStep);
    position = ysMath::Add(position, ysMath::Mul(m_velocity, vTimeStep));

    Transform.SetOrientation(orientation);
    Transform.SetPosition(position);

    ysVector acceleration = m_acceleration;
    acceleration = ysMath::Add(acceleration, ysMath::Mul(m_forceAccum, ysMath::LoadScalar(m_inverseMass)));
    ysVector angularAcceleration = ysMath::MatMult(m_inverseInertiaTensor, m_torqueAccum);

    m_velocity = 
        ysMath::Add(m_velocity, ysMath::Mul(m_impulseAccum, ysMath::LoadScalar(m_inverseMass)));
    m_angularVelocity = 
        ysMath::Add(m_angularVelocity, ysMath::Mul(m_angularImpulseAccum, ysMath::LoadScalar(m_inverseMass)));

    m_velocity = ysMath::Add(m_velocity, ysMath::Mul(acceleration, vTimeStep));
    m_angularVelocity = ysMath::Add(m_angularVelocity, ysMath::Mul(angularAcceleration, vTimeStep));

    m_angularVelocity = ysMath::Mul(m_angularVelocity, ysMath::LoadScalar(pow(m_angularDamping, timeStep)));
    m_velocity = ysMath::Mul(m_velocity, ysMath::LoadScalar(pow(m_linearDamping, timeStep)));

    int childCount = m_children.GetNumObjects();
    for (int i = 0; i < childCount; i++) {
        m_children[i]->Integrate(timeStep);
    }

    m_impulseAccum = ysMath::Constants::Zero;
}

void dphysics::RigidBody::UpdateDerivedData(bool force) {
    /* void */
}

void dphysics::RigidBody::CheckAwake() {
    // Determine if rigid body is awake
    ysVector d = ysMath::Sub(m_lastWorldPosition, Transform.GetWorldPosition());
    ysVector d2 = ysMath::Mask(ysMath::Mul(d, d), ysMath::Constants::MaskOffW);

    float maxMovement = ysMath::GetScalar(ysMath::MaxComponent(d2));

    if (maxMovement < 1E-4 && !IsAlwaysAwake()) {
        SetAwake(false);
    }
    else {
        SetAwake(true);
    }
}

ysVector dphysics::RigidBody::GetVelocityAtLocalPoint(const ysVector &localPoint) {
    ysVector delta = Transform.LocalToParentDirection(localPoint);

    ysVector angularComponent = ysMath::Cross(m_angularVelocity, delta);
    ysVector linearComponent = GetVelocity();
    return Transform.ParentToWorldDirection(
        ysMath::Add(angularComponent, linearComponent));
}

ysVector dphysics::RigidBody::GetVelocityAtWorldPoint(const ysVector &worldPoint) {
    ysVector delta = ysMath::Sub(
        worldPoint,
        Transform.GetWorldPosition());
    delta = Transform.WorldToParentDirection(delta);

    ysVector angularComponent = ysMath::Cross(m_angularVelocity, delta);
    ysVector linearComponent = GetVelocity();
    return Transform.ParentToWorldDirection(
        ysMath::Add(angularComponent, linearComponent));
}

ysMatrix dphysics::RigidBody::GetInverseInertiaTensorWorld() {
    ysMatrix orientation = ysMath::LoadMatrix(Transform.GetWorldOrientation());
    return ysMath::MatMult(orientation, m_inverseInertiaTensor);
}

void dphysics::RigidBody::SetInverseInertiaTensor(const ysMatrix &tensor) {
    m_inverseInertiaTensor = tensor;
}

ysMatrix dphysics::RigidBody::GetRectangleTensor(float dx, float dy) {
    float dx2 = dx * dx;
    float dy2 = dy * dy;

    float term1 = 0.0f;
    float term2 = 0.0f;
    float term3 = (12.0f * m_inverseMass) / (dx2 + dy2);

    ysVector row1 = ysMath::LoadVector(term1, 0.0f, 0.0f, 0.0f);
    ysVector row2 = ysMath::LoadVector(0.0f, term2, 0.0f, 0.0f);
    ysVector row3 = ysMath::LoadVector(0.0f, 0.0f, term3, 0.0f);
    ysVector row4 = ysMath::Constants::IdentityRow4;

    return ysMath::LoadMatrix(row1, row2, row3, row4);
}

void dphysics::RigidBody::AddChild(RigidBody *body) {
    body->m_parent = this;
    body->Transform.SetParent(&Transform);
    m_children.New() = body;
}

void dphysics::RigidBody::RemoveChild(RigidBody *child) {
    int index = m_children.Find(child);
    if (index == -1) return;

    child->m_parent = nullptr;
    child->Transform.SetParent(nullptr);
    m_children.Delete(index);
}

void dphysics::RigidBody::RequestCollisions() {
    int nGridCells = m_gridCells.GetNumObjects();

    for (int i = 0; i < nGridCells; i++) {
        m_system->ProcessGridCell(m_gridCells[i].x, m_gridCells[i].y);
    }
}

void dphysics::RigidBody::AddGridCell(int x, int y) {
    GridCell *gridCell = &m_gridCells.New();

    gridCell->x = x;
    gridCell->y = y;
}

void dphysics::RigidBody::AddAngularImpulseLocal(const ysVector &impulse) {
    ysVector impulseWorld = Transform.LocalToParentDirection(impulse);
    m_angularImpulseAccum = ysMath::Add(impulseWorld, m_angularImpulseAccum);
}

void dphysics::RigidBody::AddImpulseLocalSpace(const ysVector &impulse, const ysVector &localPoint) {
    ysVector impulseWorld = Transform.LocalToParentDirection(impulse);
    ysVector delta = Transform.LocalToParentDirection(localPoint);
    m_impulseAccum = ysMath::Add(m_impulseAccum, impulse);

    ysVector angularImpulse = ysMath::Cross(delta, impulse);
    m_angularImpulseAccum = ysMath::Add(m_angularImpulseAccum, angularImpulse);
}

void dphysics::RigidBody::AddImpulseWorldSpace(const ysVector &impulse, const ysVector &point) {
    ysVector delta = ysMath::Sub(point, Transform.GetWorldPosition());
    delta = Transform.WorldToParentDirection(delta);
    ysVector impulseParent = Transform.WorldToParentDirection(impulse);

    m_impulseAccum = ysMath::Add(m_impulseAccum, impulseParent);

    ysVector angularImpulse = ysMath::Cross(delta, impulseParent);
    m_angularImpulseAccum = ysMath::Add(m_angularImpulseAccum, angularImpulse);
}

void dphysics::RigidBody::AddForceLocalSpace(const ysVector &force, const ysVector &localPoint) {
    ysVector worldSpace = Transform.LocalToWorldSpace(localPoint);
    ysVector forceWorldSpace = Transform.LocalToWorldDirection(force);
    AddForceWorldSpace(forceWorldSpace, worldSpace);
}

void dphysics::RigidBody::AddForceWorldSpace(const ysVector &force, const ysVector &point) {
    ysVector delta = ysMath::Sub(point, Transform.GetWorldPosition());

    m_forceAccum = ysMath::Add(m_forceAccum, force);
    AddTorque(ysMath::Cross(force, delta));
}

void dphysics::RigidBody::AddTorqueLocal(const ysVector &torque) {
    m_torqueAccum = ysMath::Add(m_torqueAccum, torque);
}

void dphysics::RigidBody::GenerateForces(float dt) {
    //for (int j = 0; j < 8; ++j) {
        ClearAccumulators();

        int generatorCount = m_forceGenerators.GetNumObjects();
        for (int i = 0; i < generatorCount; ++i) {
            m_forceGenerators.Get(i)->GenerateForces(dt);
        }

        //Integrate(dt);
        //UpdateDerivedData();
    //}
}

bool dphysics::RigidBody::CheckState() {
    if (!Transform.IsValid()) return false;
    if (!ysMath::IsValid(m_acceleration)) return false;
    if (!ysMath::IsValid(m_angularVelocity)) return false;
    if (!ysMath::IsValid(m_forceAccum)) return false;
    if (!ysMath::IsValid(m_impulseAccum)) return false;
    if (!ysMath::IsValid(m_torqueAccum)) return false;
    if (!ysMath::IsValid(m_velocity)) return false;
    return true;
}

dphysics::Collision *dphysics::RigidBody::FindMatchingCollision(Collision *collision) {
    int collisionCount = m_collisions.GetNumObjects();
    for (int i = 0; i < collisionCount; ++i) {
        if (m_collisions[i]->IsSameAs(collision)) return m_collisions[i];
    }
    
    return nullptr;
}

void dphysics::RigidBody::ClearAccumulators() {
    ClearForceAccumulator(); 
    ClearTorqueAccumulator();
    ClearAngularImpulseAccumulator();
    ClearImpulseAccumulator();
}

void dphysics::RigidBody::WriteInfo(std::fstream &target) {
    target << "<RigidBody>" << "\n";

    int collisionObjects = m_collisionGeometry.GetNumObjects();
    int count = 0;
    for (int i = 0; i < collisionObjects; ++i) {
        CollisionObject *object = m_collisionGeometry.GetCollisionObject(i);
        if (object->GetMode() == CollisionObject::Mode::Fine) {
            object->WriteInfo(target);
        }
    }

    target << "</RigidBody>" << "\n";
}
