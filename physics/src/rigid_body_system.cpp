#include "../include/rigid_body_system.h"

#include <process.h>
#include <Windows.h>
#include <ctime>

float dphysics::RigidBodySystem::ResolutionPenetrationEpsilon = 10e-3f;

dphysics::RigidBodySystem::RigidBodySystem() : ysObject("RigidBodySystem") {
    m_currentStep = 0.1f;
    m_lastLoadMeasurement = 0;
    m_loadMeasurement = 0;
}

dphysics::RigidBodySystem::~RigidBodySystem() {
    /* void */
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

void dphysics::RigidBodySystem::GenerateCollisions(int start, int count) {
    const int REQUEST_THRESHOLD = 1;

    for (int cell = start; cell < (start + count); cell++) {
        GridCell *gridCell = &m_gridPartitionSystem.m_gridCells[cell];

        if (gridCell->m_valid && (gridCell->m_forceProcess || gridCell->GetRequestCount() >= REQUEST_THRESHOLD)) {
            int cellObjects = gridCell->m_objects.GetNumObjects();

            for (int i = 0; i < cellObjects; i++) {
                RigidBody *body1, *body2;
                body1 = gridCell->m_objects[i];

                for (int j = i + 1; j < cellObjects; j++) {
                    body2 = gridCell->m_objects[j];

                    if (body1->GetRoot() == body2->GetRoot()) continue;

                    GenerateCollisions(body1, body2);
                }
            }

            gridCell->m_processed = true;
        }
    }
}

void dphysics::RigidBodySystem::GenerateCollisions(RigidBody *body1, RigidBody *body2) {
    int nPrim1 = body1->CollisionGeometry.GetNumObjects();
    int nPrim2 = body2->CollisionGeometry.GetNumObjects();

    RigidBody *body1Ord = NULL;
    RigidBody *body2Ord = NULL;

    bool coarseCollision = true;
    bool coarsePresent = false;

    CollisionObject **object1Prims = body1->CollisionGeometry.GetCollisionObjects();
    CollisionObject **object2Prims = body2->CollisionGeometry.GetCollisionObjects();

    for (int i = 0; i < nPrim1; i++) {
        for (int j = 0; j < nPrim2; j++) {
            CollisionObject *prim1 = object1Prims[i];
            CollisionObject *prim2 = object2Prims[j];
            body1Ord = body1;
            body2Ord = body2;

            // Check whether these objects have compatible collision layers/masks
            if (!prim1->CheckCollisionMask(prim2)) continue;

            Collision newCollision;

            CollisionObject::Mode mode1 = prim1->GetMode();
            CollisionObject::Mode mode2 = prim2->GetMode();

            if (mode1 == mode2) {
                bool sensorTest = mode1 == CollisionObject::Mode::Sensor;

                OrderPrimitives(&prim1, &prim2, &body1Ord, &body2Ord);

                if (!coarseCollision && coarsePresent && mode1 == CollisionObject::Mode::Fine) continue;

                if (mode1 == CollisionObject::Mode::Fine) {
                    if (prim1->GetType() == CollisionObject::Type::Circle) {
                        if (prim2->GetType() == CollisionObject::Type::Circle) {
                            bool collisionValid = CollisionDetector.CircleCircleCollision(
                                newCollision, 
                                body1Ord->GetRoot(), body2Ord->GetRoot(), 
                                prim1->GetAsCircle(), prim2->GetAsCircle());

                            if (collisionValid) {
                                Collision *newCollisionEntry = m_dynamicCollisions.NewGeneric<Collision, 16>();
                                m_collisionAccumulator.New() = newCollisionEntry;

                                body1->AddCollision(newCollisionEntry);
                                body2->AddCollision(newCollisionEntry);

                                *newCollisionEntry = newCollision;

                                newCollisionEntry->m_collisionObject1 = prim1;
                                newCollisionEntry->m_collisionObject2 = prim2;
                                newCollisionEntry->m_sensor = sensorTest;
                            }
                        }
                        else if (prim2->GetType() == CollisionObject::Type::Box) {
                            bool collisionValid = CollisionDetector.CircleBoxCollision(
                                newCollision, 
                                body1Ord->GetRoot(), body2Ord->GetRoot(), 
                                prim1->GetAsCircle(), prim2->GetAsBox());

                            if (collisionValid) {
                                Collision *newCollisionEntry = m_dynamicCollisions.NewGeneric<Collision, 16>();
                                m_collisionAccumulator.New() = newCollisionEntry;

                                body1->AddCollision(newCollisionEntry);
                                body2->AddCollision(newCollisionEntry);

                                *newCollisionEntry = newCollision;

                                newCollisionEntry->m_collisionObject1 = prim1;
                                newCollisionEntry->m_collisionObject2 = prim2;
                                newCollisionEntry->m_sensor = sensorTest;
                            }
                        }
                    }
                }

                if (prim1->GetType() == CollisionObject::Type::Box) {
                    if (prim2->GetType() == CollisionObject::Type::Box) {
                        bool collisionValid = CollisionDetector.BoxBoxCollision(
                            newCollision, 
                            body1Ord->GetRoot(), body2Ord->GetRoot(), 
                            prim1->GetAsBox(), prim2->GetAsBox());

                        if (collisionValid) {
                            Collision *newCollisionEntry = m_dynamicCollisions.NewGeneric<Collision, 16>();
                            m_collisionAccumulator.New() = newCollisionEntry;

                            body1->AddCollision(newCollisionEntry);
                            body2->AddCollision(newCollisionEntry);

                            *newCollisionEntry = newCollision;

                            newCollisionEntry->m_collisionObject1 = prim1;
                            newCollisionEntry->m_collisionObject2 = prim2;
                            newCollisionEntry->m_sensor = sensorTest;
                        }
                    }
                }

                if (mode1 == CollisionObject::Mode::Sensor && mode2 == CollisionObject::Mode::Sensor) {
                    if (prim1->GetType() == CollisionObject::Type::Circle) {
                        if (prim2->GetType() == CollisionObject::Type::Circle) {
                            bool sense = CollisionDetector.CircleCircleIntersect(
                                body1Ord->GetRoot(), body2Ord->GetRoot(), 
                                prim1->GetAsCircle(), prim2->GetAsCircle());

                            if (sense) {
                                Collision *newCollisionEntry = m_dynamicCollisions.NewGeneric<Collision, 16>();
                                newCollisionEntry->m_body1 = body1Ord;
                                newCollisionEntry->m_body2 = body2Ord;

                                m_collisionAccumulator.New() = newCollisionEntry;
                                body1->AddCollision(newCollisionEntry);
                                body2->AddCollision(newCollisionEntry);

                                newCollisionEntry->m_collisionObject1 = prim1;
                                newCollisionEntry->m_collisionObject2 = prim2;
                                newCollisionEntry->m_sensor = sensorTest;
                            }

                            if (mode1 == CollisionObject::Mode::Coarse) {
                                coarsePresent = true;
                                coarseCollision = sense;
                            }
                        }
                        else if (prim2->GetType() == CollisionObject::Type::Ray) {
                            bool collisionValid = CollisionDetector.RayCircleCollision(
                                newCollision, 
                                body2Ord->GetRoot(), body1Ord->GetRoot(), 
                                prim2->GetAsRay(), prim1->GetAsCircle());

                            if (collisionValid) {
                                Collision *newCollisionEntry = m_dynamicCollisions.NewGeneric<Collision, 16>();
                                m_collisionAccumulator.New() = newCollisionEntry;

                                body1->AddCollision(newCollisionEntry);
                                body2->AddCollision(newCollisionEntry);

                                *newCollisionEntry = newCollision;

                                newCollisionEntry->m_collisionObject1 = prim1;
                                newCollisionEntry->m_collisionObject2 = prim2;
                                newCollisionEntry->m_sensor = sensorTest;
                            }
                        }
                    }
                }
            }
        }
    }
}

#define sgn(x) ( ((x) > 0.0f) ? 1.0f : -1.0f )

void dphysics::RigidBodySystem::GenerateCollisions() {
    m_collisionAccumulator.Clear();
    int nObjects = m_rigidBodyRegistry.GetNumObjects();

    // Generate grid cells
    m_gridPartitionSystem.Reset();
    for (int i = 0; i < nObjects; i++) {
        m_gridPartitionSystem.ProcessRigidBody(m_rigidBodyRegistry.Get(i));
    }

    for (int i = 0; i < nObjects; i++) {
        m_rigidBodyRegistry.Get(i)->ClearCollisions();
        m_rigidBodyRegistry.Get(i)->CollisionGeometry.UpdatePrimitives();
    }

    m_dynamicCollisions.Clear();
    m_collisionAccumulator.Clear();

    GenerateCollisions(0, m_gridPartitionSystem.m_gridCells.GetNumObjects());

    char buffer[1024];
    int load = m_loadMeasurement;

    // Simple tuning algorithm
    if (load > m_lastLoadMeasurement) {
        m_currentStep = -1.0f * sgn(m_currentStep) * 0.1f;
    }
    else {
        m_currentStep *= 2.0f;
        if (m_currentStep >= 1.0f) m_currentStep = 1.0f;
    }

    m_lastLoadMeasurement = m_loadMeasurement;

    //m_gridPartitionSystem.SetGridCellSize(m_gridPartitionSystem.GetGridCellSize() + m_currentStep);

    sprintf_s(buffer, 1024, "%d\t%d\t%f\n", nObjects, load, m_gridPartitionSystem.GetGridCellSize());
    m_loggingOutput.write(buffer, strlen(buffer));

    Collision collisions[16];
    int nLinks = m_rigidBodyLinks.GetNumObjects();
    for (int i = 0; i < nLinks; i++) {
        int nGenerated = m_rigidBodyLinks.Get(i)->GenerateCollisions(collisions);
        for (int j = 0; j < nGenerated; j++) {
            Collision *collision = m_collisionAccumulator.New();
            m_collisionAccumulator.New() = collision;
            *collision = collisions[j];

            collision->m_body1->AddCollision(collision);
            collision->m_body2->AddCollision(collision);
        }
    }
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
        if (collision->m_bodies[b] != NULL) {
            if (angularMove[b] != ((float)0.0)) {
                ysVector t = ysMath::Cross(collision->m_relativePosition[b], collision->m_normal);

                ysMatrix inverseInertiaTensor;
                inverseInertiaTensor = collision->m_bodies[b]->GetInverseInertiaTensorWorld();
                rotationDirection[b] = ysMath::MatMult(inverseInertiaTensor, t);

                rotationAmount[b] = angularMove[b] / angularInertia[b];
            }
            else {
                rotationDirection[b] = ysMath::Constants::Zero;
                rotationAmount[b] = 1;
            }

            velocityChange[b] = collision->m_normal;
            velocityChange[b] = ysMath::Mul(velocityChange[b], ysMath::LoadScalar(linearMove[b] / rotationAmount[b]));

            ysVector pos;
            pos = collision->m_bodies[b]->GetPosition();
            pos = ysMath::Add(pos, ysMath::Mul(collision->m_normal, ysMath::LoadScalar(linearMove[b])));
            collision->m_bodies[b]->SetPosition(pos);

            ysQuaternion q;
            q = collision->m_bodies[b]->GetOrientation();
            q = ysMath::QuatAddScaled(q, rotationDirection[b], -rotationAmount[b]);
            collision->m_bodies[b]->SetOrientation(q);
        }
    }
}

void dphysics::RigidBodySystem::ResolveCollisions() {
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

            collision.UpdateInternals();

            if (collision.m_penetration > max) {
                max = collision.m_penetration;
                index = i;
            }
        }

        if (index == numContacts) return;

        m_collisionAccumulator[index]->UpdateInternals();
        ResolveCollision(m_collisionAccumulator[index], velocityChange, rotationChange, rotationAmount, max);

        for (i = 0; i < numContacts; i++) {
            Collision &collision = *m_collisionAccumulator[i];

            if (collision.m_sensor) continue;

            collision.UpdateInternals();

            if (m_collisionAccumulator[i]->m_body1 == m_collisionAccumulator[index]->m_body1) {
                cp = ysMath::Cross(rotationChange[0], m_collisionAccumulator[i]->m_relativePosition[0]);
                cp = ysMath::Add(cp, velocityChange[0]);

                m_collisionAccumulator[i]->m_penetration -= rotationAmount[0] * ysMath::GetScalar(ysMath::Dot(cp, m_collisionAccumulator[i]->m_normal));
            }
            else if (m_collisionAccumulator[i]->m_body1 == m_collisionAccumulator[index]->m_body2) {
                cp = ysMath::Cross(rotationChange[1], m_collisionAccumulator[i]->m_relativePosition[0]);
                cp = ysMath::Add(cp, velocityChange[1]);

                m_collisionAccumulator[i]->m_penetration -= rotationAmount[1] * ysMath::GetScalar(ysMath::Dot(cp, m_collisionAccumulator[i]->m_normal));
            }

            if (m_collisionAccumulator[i]->m_body2 != nullptr) {
                if (m_collisionAccumulator[i]->m_body2 == m_collisionAccumulator[index]->m_body1) {
                    cp = ysMath::Cross(rotationChange[0], m_collisionAccumulator[i]->m_relativePosition[1]);
                    cp = ysMath::Add(cp, velocityChange[0]);

                    m_collisionAccumulator[i]->m_penetration += rotationAmount[0] * ysMath::GetScalar(ysMath::Dot(cp, m_collisionAccumulator[i]->m_normal));
                }
                else if (m_collisionAccumulator[i]->m_body2 == m_collisionAccumulator[index]->m_body2) {
                    cp = ysMath::Cross(rotationChange[1], m_collisionAccumulator[i]->m_relativePosition[1]);
                    cp = ysMath::Add(cp, velocityChange[1]);

                    m_collisionAccumulator[i]->m_penetration += rotationAmount[1] * ysMath::GetScalar(ysMath::Dot(cp, m_collisionAccumulator[i]->m_normal));
                }
            }
        }

        iterationsUsed++;
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
    int nObjects = m_rigidBodyRegistry.GetNumObjects();
    for (int i = 0; i < nObjects; i++) {
        m_rigidBodyRegistry.Get(i)->UpdateDerivedData();
    }
}

void dphysics::RigidBodySystem::Update(float timeStep) {
    Integrate(timeStep);
    GenerateCollisions();
    ResolveCollisions();
    UpdateDerivedData();
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
