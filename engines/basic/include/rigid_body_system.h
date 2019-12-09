#ifndef DELTA_BASIC_RIGID_BODY_SYSTEM_H
#define DELTA_BASIC_RIGID_BODY_SYSTEM_H

#include "delta_core.h"

#include "collision_object.h"
#include "rigid_body.h"
#include "collision_detector.h"
#include "rigid_body_link.h"
#include "grid_partition_system.h"

#include <Windows.h>
#include <fstream>

namespace dbasic {

    class DeltaEngine;

#define N_THREADS 8

    class RigidBodySystem : public ysObject {
    public:
        static const int RESOLUTION_ITERATION_LIMIT = 1000000;
        static float RESOLUTION_PENETRATION_EPSILON;

        struct CollisionGenerationCallData {
            RigidBodySystem *System;
            int Start;
            int Count;
            int ThreadID;
        };

    public:
        RigidBodySystem();
        ~RigidBodySystem();

        // Collision Detector Interface
        CollisionDetector CollisionDetector;

        void RegisterRigidBody(RigidBody *body);
        void RemoveRigidBody(RigidBody *body);

        void Update(float timeStep);

        void DrawCollisionDebug(int layer);

        void SetEngine(DeltaEngine *engine) { m_engine = engine; }

        template<typename TYPE>
        TYPE *CreateLink(RigidBody *body1, RigidBody *body2) {

            TYPE *newLink = m_rigidBodyLinks.NewGeneric<TYPE>(16);
            newLink->SetRigidBodies(body1, body2);
            return newLink;

        }

        void DeleteLink(RigidBodyLink *link);

        void ProcessGridCell(int x, int y);

    protected:
        void GenerateCollisions();
        void GenerateCollisions(RigidBody *body1, RigidBody *body2, int threadID);

        void ResolveCollisions();
        void ResolveCollision(Collision *collision, ysVector *velocityChange, ysVector *rotationDirection, float rotationAmount[2], float penetration);

        void Integrate(float timeStep);
        void UpdateDerivedData();

        void OrderPrimitives(CollisionObject **prim1, CollisionObject **prim2, RigidBody **body1, RigidBody **body2);

        static DWORD WINAPI CollisionGenerationThreadEntryPoint(void *parameters);
        void GenerateCollisions(int start, int count, int threadID);

    protected:
    public:
        DeltaEngine *m_engine;

        ysRegistry<RigidBody, 512> m_rigidBodyRegistry;

        ysDynamicArray<RigidBodyLink, 512> m_rigidBodyLinks;

        ysDynamicArray<Collision, 4> m_dynamicCollisions;
        ysExpandingArray<Collision, N_THREADS, 16> m_threadCollisionAccumulator[N_THREADS + 1];
        ysExpandingArray<Collision *, 8192> m_collisionAccumulator;
        HANDLE m_startEvents[N_THREADS];
        HANDLE m_doneEvents[N_THREADS];
        int m_comparisonCount[N_THREADS];

        bool done[8];
        bool start[8];
        int m_lastLoadMeasurement;
        int m_loadMeasurement;
        float m_currentStep;

        // TEST
        GridPartitionSystem m_gridPartitionSystem;
        std::ofstream m_loggingOutput;
    };

} /* namespace dbasic */

#endif /* DELTA_BASIC_RIGID_BODY_SYSTEM_H */
