#include "../include/rigid_body.h"

#include "../include/rigid_body_system.h"

dphysics::RigidBody::RigidBody() {
    m_linearDamping = 0.005f;
    m_angularDamping = 0.005f;

    m_position = ysMath::Constants::Zero;
    m_velocity = ysMath::Constants::Zero;
    m_angularVelocity = ysMath::Constants::Zero;
    m_orientation = ysMath::Constants::QuatIdentity;

    m_inverseInertiaTensor = ysMath::LoadMatrix(ysMath::Constants::Zero, ysMath::Constants::Zero, ysMath::Constants::Zero, ysMath::Constants::IdentityRow4);

    m_transform = ysMath::LoadIdentity();
    m_inverseTransform = ysMath::LoadIdentity();

    CollisionGeometry.SetParent(this);

    m_parent = nullptr;

    m_derivedValid = false;
    m_registered = false;

    m_owner = nullptr;
    m_hint = RigidBodyHint::Static;

    ClearAccumulators();
    m_acceleration = ysMath::Constants::Zero;
}

dphysics::RigidBody::~RigidBody() {
    /* void */
}

void dphysics::RigidBody::Integrate(float timeStep) {
    if (m_hint == RigidBodyHint::Static) return;
    else m_derivedValid = false;

    ysVector acceleration = m_acceleration;
    acceleration = ysMath::Add(acceleration, ysMath::Mul(m_forceAccum, ysMath::LoadScalar(m_inverseMass)));
    ysVector angularAcceleration = ysMath::MatMult(m_inverseInertiaTensorWorld, m_torqueAccum);

    ysVector vTimeStep = ysMath::LoadScalar(timeStep);

    m_velocity = ysMath::Add(m_velocity, ysMath::Mul(acceleration, vTimeStep));
    m_angularVelocity = ysMath::Add(m_angularVelocity, ysMath::Mul(angularAcceleration, vTimeStep));

    m_orientation = ysMath::QuatAddScaled(m_orientation, m_angularVelocity, timeStep);
    m_position = ysMath::Add(m_position, ysMath::Mul(m_velocity, vTimeStep));

    m_angularVelocity = ysMath::Mul(m_angularVelocity, ysMath::LoadScalar(pow(m_angularDamping, timeStep)));
    m_velocity = ysMath::Mul(m_velocity, ysMath::LoadScalar(pow(m_linearDamping, timeStep)));
}

void dphysics::RigidBody::UpdateDerivedData() {
    if (!m_derivedValid) {
        m_orientation = ysMath::Normalize(m_orientation);

        if (m_parent == nullptr) {
            m_worldPosition = m_position;
            m_finalOrientation = m_orientation;
        }
        else {
            m_worldPosition = ysMath::MatMult(m_parent->m_transform, ysMath::ExtendVector(m_position));
            m_finalOrientation = ysMath::Normalize(ysMath::QuatMultiply(m_orientation, m_parent->m_finalOrientation));
        }

        ysMath::LoadMatrix(m_finalOrientation, m_worldPosition, &m_transform, &m_orientationOnly);
        m_inverseInertiaTensorWorld = ysMath::MatMult(m_inverseInertiaTensor, m_orientationOnly);

        m_inverseTransform = ysMath::OrthogonalInverse(m_transform);

        m_derivedValid = true;

        int childCount = m_children.GetNumObjects();
        for (int i = 0; i < childCount; i++) {
            m_children[i]->m_derivedValid = false;
            m_children[i]->UpdateDerivedData();
        }
    }
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
    //body->m_position = GetLocalSpace(body->GetWorldPosition());

    //body->m_orientation = ysMath::QuatMultiply(body->m_orientation, ysMath::QuatInvert(m_finalOrientation));

    body->m_parent = this;
    m_children.New() = body;
}

void dphysics::RigidBody::RemoveChild(RigidBody *child) {
    int index = m_children.Find(child);

    if (index == -1) return;

    //child->m_position = child->GetWorldPosition();
    //child->SetOrientation(ysMath::QuatMultiply(m_orientation, child->m_orientation));

    child->m_parent = NULL;
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

void dphysics::RigidBody::AddForceLocalSpace(ysVector &force, ysVector &localPoint) {
    ysVector worldSpace = GetGlobalSpace(localPoint);
    AddForceWorldSpace(force, worldSpace);
}

void dphysics::RigidBody::AddForceWorldSpace(ysVector &force, ysVector &point) {
    ysVector delta = ysMath::Sub(point, GetWorldPosition());

    m_forceAccum = ysMath::Add(m_forceAccum, force);
    AddTorque(ysMath::Cross(force, delta));
}
