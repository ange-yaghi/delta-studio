#include "../include/rigid_body_system.h"

#include <ctime>
#include <assert.h>

float dphysics::RigidBodySystem::ResolutionPenetrationEpsilon = 1e-4f;

dphysics::RigidBodySystem::RigidBodySystem() : ysObject("RigidBodySystem") {
    m_currentStep = 0.1f;
    m_lastLoadMeasurement = 0;
    m_loadMeasurement = 0;
    m_replayEnabled = false;

    m_defaultDynamicFriction = 0.5f;
    m_defaultStaticFriction = 0.5f;

    m_breakdownTimer = nullptr;
}

dphysics::RigidBodySystem::~RigidBodySystem() {
    /* void */
}

void dphysics::RigidBodySystem::InitializeFrictionTable(
    int materialCount, float defaultStaticFriction, float defaultDynamicFriction)
{
    m_dynamicFrictionTable = std::vector<std::vector<float>>(
        materialCount, std::vector<float>(materialCount, defaultDynamicFriction));
    m_staticFrictionTable = std::vector<std::vector<float>>(
        materialCount, std::vector<float>(materialCount, defaultStaticFriction));

    m_defaultDynamicFriction = defaultDynamicFriction;
    m_defaultStaticFriction = defaultStaticFriction;
}

float dphysics::RigidBodySystem::GetDynamicFriction(int material1, int material2) {
    if (material1 == -1 || material2 == -1) return m_defaultDynamicFriction;

    return m_dynamicFrictionTable[material1][material2];
}

float dphysics::RigidBodySystem::GetStaticFriction(int material1, int material2) {
    if (material1 == -1 || material2 == -1) return m_defaultStaticFriction;

    return m_staticFrictionTable[material1][material2];
}

void dphysics::RigidBodySystem::SetFriction(
    int material1, int material2, float staticFriction, float dynamicFriction)
{
    m_dynamicFrictionTable[material1][material2] = dynamicFriction;
    m_dynamicFrictionTable[material2][material1] = dynamicFriction;

    m_staticFrictionTable[material1][material2] = staticFriction;
    m_staticFrictionTable[material2][material1] = staticFriction;
}

void dphysics::RigidBodySystem::RegisterRigidBody(RigidBody *body) {
    body->m_registered = true;
    body->m_system = this;
    m_rigidBodyRegistry.Register(body);
}

void dphysics::RigidBodySystem::RemoveRigidBody(RigidBody *body) {
    if (body->m_registered) m_rigidBodyRegistry.Remove(body->GetIndex());
    body->m_registered = false;
}

void dphysics::RigidBodySystem::DeleteLink(RigidBodyLink *link) {
    m_rigidBodyLinks.Delete(link->GetIndex());
}

void dphysics::RigidBodySystem::OrderPrimitives(CollisionObject **prim1, CollisionObject **prim2, RigidBody **body1, RigidBody **body2) {
    if ((*prim1)->GetType() > (*prim2)->GetType()) {
        CollisionObject *temp = *prim1;
        *prim1 = *prim2;
        *prim2 = temp;

        RigidBody *tempBody = *body1;
        *body1 = *body2;
        *body2 = tempBody;
    }
}

void dphysics::RigidBodySystem::ProcessGridCell(int x, int y) {
    GridCell *gridCell = m_gridPartitionSystem.GetCell(x, y);
    gridCell->IncrementRequestCount();

    if (gridCell->m_valid && !gridCell->m_processed) {
        int cellObjects = gridCell->m_objects.GetNumObjects();
        for (int i = 0; i < cellObjects; i++) {
            RigidBody *body1, *body2;
            body1 = gridCell->m_objects[i];

            for (int j = i + 1; j < cellObjects; j++) {
                body2 = gridCell->m_objects[j];

                if (body1->GetRoot() == body2->GetRoot()) continue;

                GenerateCollisions(body1, body2);
                m_loadMeasurement++;
            }
        }

        gridCell->m_processed = true;
    }
}

void dphysics::RigidBodySystem::OpenReplayFile(const std::string &fname) {
    m_outputFile = std::fstream(fname, std::ios::out);

    if (!m_outputFile.is_open()) {
        return;
    }

    int bodyCount = m_rigidBodyRegistry.GetNumObjects();

    m_outputFile << "<Objects>" << "\n";

    for (int i = 0; i < bodyCount; ++i) {
        RigidBody *body = m_rigidBodyRegistry.Get(i);
        body->WriteInfo(m_outputFile);
    }

    m_outputFile << "</Objects>" << "\n";

    m_replayEnabled = true;
}

void dphysics::RigidBodySystem::CloseReplayFile() {
    m_replayEnabled = false;
    m_outputFile.close();
}

void dphysics::RigidBodySystem::GenerateCollisions(int start, int count) {
    const int REQUEST_THRESHOLD = 0;

    const int rigidBodyCount = m_rigidBodyRegistry.GetNumObjects();
    bool **visited = new bool *[rigidBodyCount];
    for (int i = 0; i < rigidBodyCount; ++i) {
        visited[i] = new bool[rigidBodyCount];
        memset((void *)visited[i], 0, sizeof(bool) * rigidBodyCount);
    }

    for (auto cell: m_gridPartitionSystem.m_gridCells) {
        GridCell *gridCell = cell.second;

        if (gridCell->m_valid && (gridCell->m_forceProcess || gridCell->GetRequestCount() >= REQUEST_THRESHOLD)) {
            int cellObjects = gridCell->m_objects.GetNumObjects();

            for (int i = 0; i < cellObjects; i++) {
                RigidBody *body1, *body2;
                body1 = gridCell->m_objects[i];

                for (int j = i + 1; j < cellObjects; j++) {
                    body2 = gridCell->m_objects[j];

                    if (visited[body1->GetIndex()][body2->GetIndex()]) continue;
                    if (body1->GetRoot() == body2->GetRoot()) continue;

                    visited[body1->GetIndex()][body2->GetIndex()] = true;

                    GenerateCollisions(body1, body2);
                }
            }

            gridCell->m_processed = true;
        }
    }

    for (int i = 0; i < rigidBodyCount; ++i) {
        delete[] visited[i];
    }
    delete[] visited;
}

void dphysics::RigidBodySystem::WriteFrameToReplayFile() {
    m_outputFile << "<Frame>" << "\n";

    int bodyCount = m_rigidBodyRegistry.GetNumObjects();
    for (int i = 0; i < bodyCount; ++i) {
        RigidBody *body = m_rigidBodyRegistry.Get(i);

        m_outputFile << "<Body>" << "\n";
        m_outputFile << "POSITION " <<
            ysMath::GetX(body->Transform.GetWorldPosition()) << " " <<
            ysMath::GetY(body->Transform.GetWorldPosition()) << " " <<
            ysMath::GetZ(body->Transform.GetWorldPosition()) << "\n";
        m_outputFile << "ORIENTATION " <<
            ysMath::GetQuatW(body->Transform.GetWorldOrientation()) << " " <<
            ysMath::GetQuatX(body->Transform.GetWorldOrientation()) << " " <<
            ysMath::GetQuatY(body->Transform.GetWorldOrientation()) << " " <<
            ysMath::GetQuatZ(body->Transform.GetWorldOrientation()) << "\n";
        m_outputFile << "</Body>" << "\n";
    }

    m_outputFile << "</Frame>" << "\n";
}

void dphysics::RigidBodySystem::GenerateCollisions(RigidBody *body1, RigidBody *body2) {
    if (!body1->IsAwake() && !body2->IsAwake()) return;

    const int nPrim1 = body1->m_collisionGeometry.GetNumObjects();
    const int nPrim2 = body2->m_collisionGeometry.GetNumObjects();

    RigidBody *body1Ord = nullptr;
    RigidBody *body2Ord = nullptr;

    bool coarseCollision = true;
    bool coarsePresent = false;

    CollisionObject **object1Prims = body1->m_collisionGeometry.GetCollisionObjects();
    CollisionObject **object2Prims = body2->m_collisionGeometry.GetCollisionObjects();

    for (int i = 0; i < nPrim1; i++) {
        for (int j = 0; j < nPrim2; j++) {
            CollisionObject *prim1 = object1Prims[i];
            CollisionObject *prim2 = object2Prims[j];
            body1Ord = body1;
            body2Ord = body2;

            // Check whether these objects have compatible collision layers/masks
            if (!prim1->CheckCollisionMask(prim2)) continue;

            int nCollisions = 0;
            Collision newCollisions[4];

            CollisionObject::Mode mode1 = prim1->GetMode();
            CollisionObject::Mode mode2 = prim2->GetMode();

            if (mode1 == mode2) {
                bool sensorTest = mode1 == CollisionObject::Mode::Sensor;
                if (sensorTest &&
                    !body1Ord->RequestsInformation() &&
                    !body2Ord->RequestsInformation())
                {
                    continue;
                }

                OrderPrimitives(&prim1, &prim2, &body1Ord, &body2Ord);

                if (!coarseCollision && coarsePresent && mode1 == CollisionObject::Mode::Fine) continue;

                if (mode1 == CollisionObject::Mode::Fine) {
                    if (prim1->GetType() == CollisionObject::Type::Circle) {
                        if (prim2->GetType() == CollisionObject::Type::Circle) {
                            nCollisions = m_collisionDetector.CircleCircleCollision(
                                newCollisions,
                                body1Ord->GetRoot(), body2Ord->GetRoot(),
                                prim1->GetAsCircle(), prim2->GetAsCircle());
                        }
                        else if (prim2->GetType() == CollisionObject::Type::Box) {
                            nCollisions = m_collisionDetector.CircleBoxCollision(
                                newCollisions,
                                body1Ord->GetRoot(), body2Ord->GetRoot(),
                                prim1->GetAsCircle(), prim2->GetAsBox());
                        }
                    }
                }

                if (prim1->GetType() == CollisionObject::Type::Box) {
                    if (prim2->GetType() == CollisionObject::Type::Box) {
                        nCollisions = m_collisionDetector.BoxBoxCollision(
                            newCollisions,
                            body1Ord->GetRoot(), body2Ord->GetRoot(),
                            prim1->GetAsBox(), prim2->GetAsBox());
                    }
                }

                if (mode1 == CollisionObject::Mode::Sensor &&
                    mode2 == CollisionObject::Mode::Sensor)
                {
                    if (prim1->GetType() == CollisionObject::Type::Circle) {
                        if (prim2->GetType() == CollisionObject::Type::Circle) {
                            bool intersect = m_collisionDetector.CircleCircleIntersect(
                                body1Ord->GetRoot(), body2Ord->GetRoot(),
                                prim1->GetAsCircle(), prim2->GetAsCircle());

                            if (intersect) nCollisions = 1;
                            newCollisions[0].m_body1 = body1Ord;
                            newCollisions[0].m_body2 = body2Ord;
                            newCollisions[0].m_collisionObject1 = prim1;
                            newCollisions[0].m_collisionObject2 = prim2;

                            if (mode1 == CollisionObject::Mode::Coarse) {
                                coarsePresent = true;
                                coarseCollision = nCollisions > 0;
                            }
                        }
                        else if (prim2->GetType() == CollisionObject::Type::Ray) {
                            nCollisions = m_collisionDetector.RayCircleCollision(
                                newCollisions,
                                body2Ord->GetRoot(), body1Ord->GetRoot(),
                                prim2->GetAsRay(), prim1->GetAsCircle());
                        }
                    }
                }

                for (int i = 0; i < nCollisions; ++i) {
                    Collision *newCollisionEntry = m_dynamicCollisions.NewGeneric<Collision, 16>();
                    m_collisionAccumulator.New() = newCollisionEntry;

                    *newCollisionEntry = newCollisions[i];

                    if (!sensorTest || body1->RequestsInformation()) {
                        body1->AddCollision(newCollisionEntry);
                    }

                    if (!sensorTest || body2->RequestsInformation()) {
                        body2->AddCollision(newCollisionEntry);
                    }

                    newCollisionEntry->m_collisionObject1 = prim1;
                    newCollisionEntry->m_collisionObject2 = prim2;
                    newCollisionEntry->m_sensor = sensorTest;
                    newCollisionEntry->m_dynamicFriction =
                        GetDynamicFriction(body1->GetMaterial(), body2->GetMaterial());
                    newCollisionEntry->m_staticFriction =
                        GetStaticFriction(body1->GetMaterial(), body2->GetMaterial());
                }
            }
        }
    }
}

#define sgn(x) ( ((x) > 0.0f) ? 1.0f : -1.0f )

bool dphysics::RigidBodySystem::CollisionExists(Collision *collision) {
    return collision->m_body1->FindMatchingCollision(collision) != nullptr;
}

void dphysics::RigidBodySystem::GenerateCollisions() {
    ClearCollisions();
    int nObjects = m_rigidBodyRegistry.GetNumObjects();

    // Generate grid cells
    m_gridPartitionSystem.Reset();
    for (int i = 0; i < nObjects; i++) {
        m_gridPartitionSystem.ProcessRigidBody(m_rigidBodyRegistry.Get(i));
    }

    for (int i = 0; i < nObjects; i++) {
        m_rigidBodyRegistry.Get(i)->ClearCollisions();
        m_rigidBodyRegistry.Get(i)->m_collisionGeometry.UpdatePrimitives();
    }

    GenerateCollisions(0, 0);

    const int load = m_loadMeasurement;

    // Simple tuning algorithm
    if (load > m_lastLoadMeasurement) {
        m_currentStep = -1.0f * sgn(m_currentStep) * 0.1f;
    }
    else {
        m_currentStep *= 2.0f;
        if (m_currentStep >= 1.0f) m_currentStep = 1.0f;
    }

    m_lastLoadMeasurement = m_loadMeasurement;

    Collision collisions[16];
    int nLinks = m_rigidBodyLinks.GetNumObjects();
    for (int i = 0; i < nLinks; i++) {
        int nGenerated = m_rigidBodyLinks.Get(i)->GenerateCollisions(collisions);
        for (int j = 0; j < nGenerated; j++) {
            Collision *newCollisionEntry = m_dynamicCollisions.NewGeneric<Collision, 16>();
            m_collisionAccumulator.New() = newCollisionEntry;
            *newCollisionEntry = collisions[j];

            newCollisionEntry->m_body1->AddCollision(newCollisionEntry);
            newCollisionEntry->m_body2->AddCollision(newCollisionEntry);
        }
    }
}

void dphysics::RigidBodySystem::InitializeCollisions() {
    int numContacts = m_collisionAccumulator.GetNumObjects();
    for (int i = 0; i < numContacts; ++i) {
        Collision &collision = *m_collisionAccumulator[i];

        if (collision.m_sensor) continue;
        if (collision.IsGhost()) continue;

        collision.Initialize();
    }
}

void dphysics::RigidBodySystem::CleanCollisions() {
    constexpr float MinPenetration = -0.1f;

    int numContacts = m_collisionAccumulator.GetNumObjects();
    for (int i = 0; i < numContacts; ++i) {
        Collision &collision = *m_collisionAccumulator[i];

        if (collision.m_penetration < MinPenetration ||
            collision.IsGhost() ||
            collision.m_sensor)
        {
            m_collisionAccumulator.Delete(i, false);
            m_dynamicCollisions.Delete(collision.GetIndex());
            --numContacts;
        }
    }
}

void dphysics::RigidBodySystem::ClearCollisions() {
    m_collisionAccumulator.Clear();
    m_dynamicCollisions.Clear();
}

void dphysics::RigidBodySystem::ResolveCollision(Collision *collision, ysVector *velocityChange, ysVector *rotationDirection, float rotationAmount[2], float penetration) {
    float angularLimit = 0.1f;
    float angularMove[2], linearMove[2];
    int b;

    float totalInertia = 0.0f;
    float linearInertia[2] = {0.0f, 0.0f};
    float angularInertia[2] = {0.0f, 0.0f};

    for (unsigned i = 0; i < 2; i++) {
        if (collision->m_bodies[i] != nullptr) {
            ysMatrix inverseInertiaTensor = collision->m_bodies[i]->GetInverseInertiaTensorWorld();

            // Use the same procedure as for calculating frictionless
            // velocity change to work out the angular inertia.
            ysVector angularInertiaWorld = ysMath::Cross(collision->m_relativePosition[i], collision->m_normal);
            angularInertiaWorld = ysMath::MatMult(inverseInertiaTensor, angularInertiaWorld);
            angularInertiaWorld = ysMath::Cross(angularInertiaWorld, collision->m_relativePosition[i]);
            angularInertia[i] = ysMath::GetScalar(ysMath::Dot(angularInertiaWorld, collision->m_normal));

            // The linear component is simply the inverse mass
            linearInertia[i] = collision->m_bodies[i]->GetInverseMass();

            // Keep track of the total inertia from all components
            totalInertia += linearInertia[i] + angularInertia[i];
        }
    }

    float inverseMass[2];
    totalInertia = angularInertia[0];
    if (collision->m_bodies[0] != nullptr) {
        totalInertia += collision->m_bodies[0]->GetInverseMass();
    }

    if (collision->m_bodies[1] != nullptr) {
        inverseMass[1] = angularInertia[1] + collision->m_bodies[1]->GetInverseMass();
        totalInertia += inverseMass[1];

        angularMove[1] = -collision->m_penetration * angularInertia[1] / totalInertia;
        linearMove[1] = -collision->m_penetration * collision->m_bodies[1]->GetInverseMass() / totalInertia;

        // To avoid angular projections that are too great (when mass is large
        // but inertia tensor is small) limit the angular move.
        ysVector projection = collision->m_relativePosition[1];
        projection = ysMath::Sub(projection, ysMath::Mul(collision->m_normal, ysMath::Dot(collision->m_relativePosition[1], collision->m_normal)));

        float max = angularLimit * ysMath::GetScalar(ysMath::Magnitude(collision->m_relativePosition[0]));

        if (abs(angularMove[1]) > max) {
            float pp = angularMove[1] + linearMove[1];
            angularMove[1] = angularMove[1] > 0 ? max : -max;
            linearMove[1] = pp - angularMove[1];
        }
    }

    angularMove[0] = collision->m_penetration * angularInertia[0] / totalInertia;
    linearMove[0] = collision->m_penetration * collision->m_bodies[0]->GetInverseMass() / totalInertia;

    // To avoid angular projections that are too great (when mass is large
    // but inertia tensor is small) limit the angular move.
    ysVector projection = collision->m_relativePosition[0];
    projection = ysMath::Sub(projection, ysMath::Mul(collision->m_normal, ysMath::Dot(collision->m_relativePosition[0], collision->m_normal)));

    float max = angularLimit * ysMath::GetScalar(ysMath::Magnitude(collision->m_relativePosition[0]));

    if (abs(angularMove[0]) > max) {
        float pp = angularMove[0] + linearMove[0];
        angularMove[0] = angularMove[0] > 0 ? max : -max;
        linearMove[0] = pp - angularMove[0];
    }

    for (b = 0; b < 2; b++) {
        if (collision->m_bodies[b] != nullptr) {
            RigidBody *body = collision->m_bodies[b];

            if (angularMove[b] != ((float)0.0)) {
                ysVector t = ysMath::Cross(collision->m_relativePosition[b], collision->m_normal);

                ysMatrix inverseInertiaTensor;
                inverseInertiaTensor = body->GetInverseInertiaTensorWorld();
                rotationDirection[b] = ysMath::MatMult(inverseInertiaTensor, t);
                rotationAmount[b] = angularMove[b] / angularInertia[b];
            }
            else {
                rotationDirection[b] = ysMath::Constants::Zero;
                rotationAmount[b] = 1;
            }

            velocityChange[b] = collision->m_normal;
            velocityChange[b] = ysMath::Mul(velocityChange[b], ysMath::LoadScalar(linearMove[b] / rotationAmount[b]));

            ysVector pos = body->Transform.GetPositionParentSpace();
            pos = ysMath::Add(pos, ysMath::Mul(collision->m_normal, ysMath::LoadScalar(linearMove[b])));
            body->Transform.SetPosition(pos);

            ysQuaternion q = body->Transform.GetOrientationParentSpace();
            q = ysMath::QuatAddScaled(q, rotationDirection[b], rotationAmount[b] * 0.5f);
            body->Transform.SetOrientation(q);
        }
    }
}

void dphysics::RigidBodySystem::AdjustVelocities(float timestep) {
    const int numContacts = m_collisionAccumulator.GetNumObjects();

    ysVector velocityChange[2], rotationChange[2];
    ysVector cp;

    // iteratively handle impacts in order of severity.
    for (int velocityIterationsUsed = 0; velocityIterationsUsed < ResolutionIterationLimit; ++velocityIterationsUsed) {
        // Find contact with maximum magnitude of probable velocity change.
        float max = 1E-4f;
        int index = numContacts;
        for(int i = 0; i < numContacts; ++i) {
            Collision &collision = *m_collisionAccumulator[i];
            if (collision.m_desiredDeltaVelocity > max) {
                if (collision.m_sensor) continue;
                if (collision.IsGhost()) continue;
                if (!collision.IsResolvable()) continue;

                max = collision.m_desiredDeltaVelocity;
                index = i;
            }
        }
        if (index == numContacts) break;

        Collision *biggestCollision = m_collisionAccumulator[index];

        // Match the awake state at the contact
        //c[index].matchAwakeState();

        // Do the resolution on the contact with the largest desired velocity
        AdjustVelocity(biggestCollision, velocityChange, rotationChange);

        // With the change in velocity of the two bodies, the update of
        // contact velocities means that some of the relative closing
        // velocities need recomputing.
        for (int i = 0; i < numContacts; ++i) {
            Collision *c = m_collisionAccumulator[i];

            if (c->m_sensor) continue;
            if (c->IsGhost()) continue;
            if (!c->IsResolvable())
                continue;

            if (c->m_bodies[0] != nullptr) {
                if (c->m_bodies[0] == biggestCollision->m_bodies[0]) {
                    cp = ysMath::Cross(rotationChange[0], c->m_relativePosition[0]);
                    cp = ysMath::Add(cp, velocityChange[0]);

                    c->m_contactVelocity = ysMath::Add(c->m_contactVelocity,
                        ysMath::MatMult(ysMath::OrthogonalInverse(c->m_contactSpace), cp));
                    c->CalculateDesiredDeltaVelocity(timestep);
                }
                else if (c->m_bodies[0] == biggestCollision->m_bodies[1]) {
                    cp = ysMath::Cross(rotationChange[1], c->m_relativePosition[0]);
                    cp = ysMath::Add(cp, velocityChange[1]);

                    c->m_contactVelocity = ysMath::Add(c->m_contactVelocity,
                        ysMath::MatMult(ysMath::OrthogonalInverse(c->m_contactSpace), cp));
                    c->CalculateDesiredDeltaVelocity(timestep);
                }
            }

            if (c->m_bodies[1] != nullptr) {
                if (c->m_bodies[1] == biggestCollision->m_bodies[0]) {
                    cp = ysMath::Cross(rotationChange[0], c->m_relativePosition[1]);
                    cp = ysMath::Add(cp, velocityChange[0]);

                    c->m_contactVelocity = ysMath::Sub(c->m_contactVelocity,
                        ysMath::MatMult(ysMath::OrthogonalInverse(c->m_contactSpace), cp));
                    c->CalculateDesiredDeltaVelocity(timestep);
                }
                else if (c->m_bodies[1] == biggestCollision->m_bodies[1]) {
                    cp = ysMath::Cross(rotationChange[1], c->m_relativePosition[1]);
                    cp = ysMath::Add(cp, velocityChange[1]);

                    c->m_contactVelocity = ysMath::Sub(c->m_contactVelocity,
                        ysMath::MatMult(ysMath::OrthogonalInverse(c->m_contactSpace), cp));
                    c->CalculateDesiredDeltaVelocity(timestep);
                }
            }
        }
    }
}

void dphysics::RigidBodySystem::AdjustVelocity(Collision *collision, ysVector velocityChange[2], ysVector rotationChange[2]) {
    // Inverse mass and inertia tensor in world coordinates
    ysMatrix inverseInertiaTensor[2];
    inverseInertiaTensor[0] = collision->m_bodies[0]->GetInverseInertiaTensorWorld();

    ysVector impulseContact;
    float inverseMass = collision->m_bodies[0]->GetInverseMass();
    ysMatrix impulseToTorque = ysMath::SkewSymmetric(collision->m_relativePosition[0]);

    // Build the matrix to convert contact impulse to change in velocity
    // in world coordinates.
    ysMatrix deltaVelWorld = impulseToTorque;
    deltaVelWorld = ysMath::MatMult(deltaVelWorld, inverseInertiaTensor[0]);
    deltaVelWorld = ysMath::MatMult(deltaVelWorld, impulseToTorque);
    deltaVelWorld = ysMath::Negate3x3(deltaVelWorld);

    // Check if we need to add body two's data
    if (collision->m_bodies[1] != nullptr) {
        // Find the inertia tensor for this body
        inverseInertiaTensor[1] = collision->m_bodies[1]->GetInverseInertiaTensorWorld();

        // Set the cross product matrix
        impulseToTorque = ysMath::SkewSymmetric(collision->m_relativePosition[1]);

        // Calculate the velocity change matrix
        ysMatrix deltaVelWorld2 = impulseToTorque;
        deltaVelWorld2 = ysMath::MatMult(deltaVelWorld2, inverseInertiaTensor[1]);
        deltaVelWorld2 = ysMath::MatMult(deltaVelWorld2, impulseToTorque);
        deltaVelWorld2 = ysMath::Negate3x3(deltaVelWorld2);

        // Add to the total delta velocity.
        deltaVelWorld = ysMath::MatAdd(deltaVelWorld2, deltaVelWorld);

        // Add to the inverse mass
        inverseMass += collision->m_bodies[1]->GetInverseMass();
    }

    // Do a change of basis to convert into contact coordinates.
    ysMatrix deltaVelocity = ysMath::OrthogonalInverse(collision->m_contactSpace);
    deltaVelocity = ysMath::MatMult(deltaVelocity, deltaVelWorld);
    deltaVelocity = ysMath::MatMult(deltaVelocity, collision->m_contactSpace);

    // Add in the linear velocity change
    deltaVelocity = ysMath::MatAdd(deltaVelocity, ysMath::ScaleTransform(ysMath::LoadScalar(inverseMass)));
    deltaVelocity = ysMath::MatConvert3x3(deltaVelocity);

    // Invert to get the impulse needed per unit velocity
    ysMatrix impulseMatrix = ysMath::Inverse3x3(deltaVelocity);

    // Find the target velocities to kill
    ysVector velKill = ysMath::LoadVector(collision->m_desiredDeltaVelocity,
        -ysMath::GetY(collision->m_contactVelocity),
        -ysMath::GetZ(collision->m_contactVelocity));

    // Find the impulse to kill target velocities
    impulseContact = ysMath::MatMult(impulseMatrix, velKill);

    // Check for exceeding friction
    ysVector planarImpulse_v = ysMath::Magnitude(ysMath::Mask(impulseContact, ysMath::Constants::MaskOffX));
    float planarImpulse = ysMath::GetScalar(planarImpulse_v);
    if (planarImpulse > std::abs(ysMath::GetX(impulseContact)) * collision->m_staticFriction) {
        ysVector ic = ysMath::Mask(impulseContact, ysMath::Constants::MaskOffX);
        ic = ysMath::Div(ic, planarImpulse_v);
        ic = ysMath::Mul(ic, ysMath::LoadScalar(collision->m_dynamicFriction));

        ysVector icx = ysMath::Dot(deltaVelocity.rows[0], ysMath::Or(ic, ysMath::Constants::XAxis));
        icx = ysMath::Div(ysMath::LoadScalar(collision->m_desiredDeltaVelocity), icx);

        ic = ysMath::Mul(ic, icx);
        icx = ysMath::Mask(icx, ysMath::Constants::MaskKeepX);

        impulseContact = ysMath::Add(ic, icx);
    }

    // Convert impulse to world coordinates
    ysVector impulse = ysMath::MatMult(collision->m_contactSpace, impulseContact);

    // Split in the impulse into linear and rotational components
    ysVector impulsiveTorque = ysMath::Cross(collision->m_relativePosition[0], impulse);
    rotationChange[0] = ysMath::MatMult(inverseInertiaTensor[0], impulsiveTorque);
    velocityChange[0] = ysMath::Mul(impulse, ysMath::LoadScalar(collision->m_bodies[0]->GetInverseMass()));

    // Apply the changes
    collision->m_bodies[0]->AddVelocity(velocityChange[0]);
    collision->m_bodies[0]->AddAngularVelocity(rotationChange[0]);

    if (collision->m_bodies[1] != nullptr) {
        // Work out body one's linear and angular changes
        ysVector impulsiveTorque = ysMath::Cross(impulse, collision->m_relativePosition[1]);
        rotationChange[1] = ysMath::MatMult(inverseInertiaTensor[1], impulsiveTorque);
        velocityChange[1] = ysMath::Mul(impulse, ysMath::LoadScalar(-collision->m_bodies[1]->GetInverseMass()));

        // And apply them.
        collision->m_bodies[1]->AddVelocity(velocityChange[1]);
        collision->m_bodies[1]->AddAngularVelocity(rotationChange[1]);
    }

    assert(ysMath::IsValid(velocityChange[0]));
    assert(ysMath::IsValid(velocityChange[1]));
    assert(ysMath::IsValid(rotationChange[0]));
    assert(ysMath::IsValid(rotationChange[1]));
}

void dphysics::RigidBodySystem::ResolveCollisions(float dt) {
    int i, index;
    int numContacts = m_collisionAccumulator.GetNumObjects();

    float max;
    int iterationsUsed = 0;

    float rotationAmount[2];
    ysVector velocityChange[2], rotationChange[2];
    ysVector cp;

    while (iterationsUsed < ResolutionIterationLimit) {
        max = ResolutionPenetrationEpsilon;
        index = numContacts;

        for (i = 0; i < numContacts; i++) {
            Collision &collision = *m_collisionAccumulator[i];

            if (collision.m_sensor) continue;
            if (collision.IsGhost()) continue;
            if (!collision.IsResolvable()) continue;

            collision.UpdateInternals(dt);

            if (collision.m_penetration > max) {
                max = collision.m_penetration;
                index = i;
            }
        }

        if (index == numContacts) return;

        Collision *biggestCollision = m_collisionAccumulator[index];

        biggestCollision->UpdateInternals(dt);
        ResolveCollision(biggestCollision,
            velocityChange,
            rotationChange,
            rotationAmount,
            max);

        for (i = 0; i < numContacts; i++) {
            Collision &collision = *m_collisionAccumulator[i];

            if (collision.m_sensor) continue;
            if (collision.IsGhost()) continue;
            if (!collision.IsResolvable())
                continue;

            if (collision.m_body1 == biggestCollision->m_body1) {
                cp = ysMath::Cross(rotationChange[0], collision.m_relativePosition[0]);
                cp = ysMath::Add(cp, velocityChange[0]);

                collision.m_penetration -=
                    rotationAmount[0] * ysMath::GetScalar(ysMath::Dot(cp, collision.m_normal));
            }
            else if (collision.m_body1 == biggestCollision->m_body2) {
                cp = ysMath::Cross(rotationChange[1], collision.m_relativePosition[0]);
                cp = ysMath::Add(cp, velocityChange[1]);

                collision.m_penetration -=
                    rotationAmount[1] * ysMath::GetScalar(ysMath::Dot(cp, collision.m_normal));
            }

            if (collision.m_body2 != nullptr) {
                if (collision.m_body2 == biggestCollision->m_body1) {
                    cp = ysMath::Cross(rotationChange[0], collision.m_relativePosition[1]);
                    cp = ysMath::Add(cp, velocityChange[0]);

                    collision.m_penetration +=
                        rotationAmount[0] * ysMath::GetScalar(ysMath::Dot(cp, collision.m_normal));
                }
                else if (collision.m_body2 == biggestCollision->m_body2) {
                    cp = ysMath::Cross(rotationChange[1], collision.m_relativePosition[1]);
                    cp = ysMath::Add(cp, velocityChange[1]);

                    collision.m_penetration +=
                        rotationAmount[1] * ysMath::GetScalar(ysMath::Dot(cp, collision.m_normal));
                }
            }
        }

        ++iterationsUsed;
    }
}

void dphysics::RigidBodySystem::GenerateForces(float timeStep) {
    int nObjects = m_rigidBodyRegistry.GetNumObjects();
    for (int i = 0; i < nObjects; i++) {
        m_rigidBodyRegistry.Get(i)->GenerateForces(timeStep);
    }
}

void dphysics::RigidBodySystem::Integrate(float timeStep) {
    int nObjects = m_rigidBodyRegistry.GetNumObjects();
    for (int i = 0; i < nObjects; i++) {
        m_rigidBodyRegistry.Get(i)->Integrate(timeStep);
        m_rigidBodyRegistry.Get(i)->UpdateDerivedData();
    }
}

void dphysics::RigidBodySystem::UpdateDerivedData() {
    const int nObjects = m_rigidBodyRegistry.GetNumObjects();
    for (int i = 0; i < nObjects; i++) {
        m_rigidBodyRegistry.Get(i)->UpdateDerivedData();
    }
}

void dphysics::RigidBodySystem::CheckAwake() {
    const int nObjects = m_rigidBodyRegistry.GetNumObjects();
    for (int i = 0; i < nObjects; i++) {
        m_rigidBodyRegistry.Get(i)->CheckAwake();
    }
}

void dphysics::RigidBodySystem::Update(float timestep) {
    //GenerateForces(timestep);

    Integrate(timestep);

    GenerateCollisions();
    InitializeCollisions();
    ResolveCollisions(timestep);
    AdjustVelocities(timestep);
    CheckAwake();

    if (m_replayEnabled) {
        WriteFrameToReplayFile();
    }
}

bool dphysics::RigidBodySystem::CheckState() {
    const int nBodies = m_rigidBodyRegistry.GetNumObjects();
    for (int i = 0; i < nBodies; ++i) {
        const bool valid = m_rigidBodyRegistry.Get(i)->CheckState();
        if (!valid) return false;
    }

    return true;
}

/*
void dphysics::RigidBodySystem::DrawCollisionDebug(int layer) {
    int nCollisions = m_collisionAccumulator.GetNumObjects();
    int color[3] = { 255, 0, 0 };

    for (int i = 0; i < nCollisions; i++) {
        Collision *collision = m_collisionAccumulator[i];

        if (collision->m_penetration < -10.0f) {
            int a = 0;
        }

        m_engine->DrawAxis(color, collision->m_position, collision->m_normal, 0.01f, collision->m_penetration, layer);
    }

    int nLinks = m_rigidBodyLinks.GetNumObjects();
    for (int i = 0; i < nLinks; i++) {
        //m_rigidBodyLinks.Get(i)->DrawDebug(m_engine, layer);
    }
}
*/
