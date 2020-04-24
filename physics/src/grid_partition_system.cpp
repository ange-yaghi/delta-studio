#include "../include/grid_partition_system.h"

#include "../include/rigid_body.h"

dphysics::GridCell::GridCell() {
    m_x = 0;
    m_y = 0;
    m_valid = false;
    m_processed = false;
    m_requestCount = 0;
    m_forceProcess = false;
    m_active = false;
}

dphysics::GridCell::~GridCell() {
    /* void */
}

void dphysics::GridCell::IncrementRequestCount() {
    m_requestCount++;

    if (m_requestCount >= 500) {
        m_requestCount = 500;
    }
}

void dphysics::GridCell::DecrementRequestCount() {
    m_requestCount--;

    if (m_requestCount <= 0) {
        m_requestCount--;
    }
}

dphysics::GridPartitionSystem::GridPartitionSystem() : ysObject("ysGridPartitionSystem") {
    m_maxCells = 8192;
    m_gridCellSize = 5.0f;
    m_maxObjectSize = 30.0f;
    m_gridCells.Allocate(m_maxCells);
}

dphysics::GridPartitionSystem::~GridPartitionSystem() {
    /* void */
}

void dphysics::GridPartitionSystem::Reset() {
    GridCell *gridCell;

    for (int i = 0; i < m_maxCells; i++) {
        gridCell = &m_gridCells[i];

        if (gridCell->m_requestCount > 0 && gridCell->m_valid) {
            // Allow this block to persist
            gridCell->m_active = true;
            gridCell->DecrementRequestCount();
        }
        else {
            gridCell->m_requestCount = 0;
            gridCell->m_active = false;
        }

        gridCell->m_forceProcess = false;
        gridCell->m_valid = false;
        gridCell->m_processed = false;
        gridCell->m_objects.Clear();
    }
}

dphysics::GridCell *dphysics::GridPartitionSystem::GetCell(int x, int y) {
    int hashIndex = GetHash(x, y);
    int originalIndex = hashIndex;
    bool notFound = false;

    GridCell *gridCell;
    while (true) {
        gridCell = &m_gridCells[hashIndex];

        if ((gridCell->m_active && (gridCell->m_x != x || gridCell->m_y != y))) {
            hashIndex++;
            hashIndex %= m_maxCells;
            if (hashIndex == originalIndex) return NULL;
            continue;
        }
        else if (gridCell->m_active) {
            gridCell->m_valid = true;
            return gridCell;
        }
        else if (!gridCell->m_active) {
            gridCell->m_x = x;
            gridCell->m_y = y;
            gridCell->m_active = true;
            gridCell->m_valid = true;
            return gridCell;
        }
    }

    return NULL;
}

int dphysics::GridPartitionSystem::GetHash(int x, int y) {
    return (((unsigned int)x) * 0x50F1A + ((unsigned int)y) * 0x7651) % m_maxCells;
}

int dphysics::GridPartitionSystem::CalculateLoad() {
    int load = 0;

    for (int i = 0; i < m_maxCells; i++) {
        if (m_gridCells[i].m_valid) {
            int count = m_gridCells[i].m_objects.GetNumObjects();
            load += count * count / 2;
        }
    }

    return load;
}

void dphysics::GridPartitionSystem::AddObject(int x, int y, RigidBody *body) {
    GridCell *gridCell = GetCell(x, y);
    gridCell->m_objects.New() = body;
    gridCell->m_forceProcess = body->GetHint() == RigidBody::RigidBodyHint::Dynamic;
    body->AddGridCell(x, y);
}

void dphysics::GridPartitionSystem::ProcessRigidBody(RigidBody *object) {
    object->ClearGridCells();

    ysVector pos = object->Transform.GetWorldPosition();

    float actual_x = ysMath::GetX(pos);
    float actual_y = ysMath::GetY(pos);

    int x = (int)(std::floor(actual_x / m_gridCellSize));
    int y = (int)(std::floor(actual_y / m_gridCellSize));

    float nominal_x = m_gridCellSize * x;
    float nominal_y = m_gridCellSize * y;

    AddObject(x, y, object);

    float threshold = (m_gridCellSize / 2.0f) - (m_maxObjectSize / 2.0f);

    float deltax = actual_x - nominal_x;
    float deltay = actual_y - nominal_y;

    if (deltax > threshold) {
        AddObject(x + 1, y, object);

        if (deltay > threshold) {
            AddObject(x + 1, y + 1, object);
        }
        else if (-deltay > threshold) {
            AddObject(x + 1, y - 1, object);
        }
    }
    if (-deltax > threshold) {
        AddObject(x - 1, y, object);

        if (deltay > threshold) {
            AddObject(x - 1, y + 1, object);
        }
        else if (-deltay > threshold) {
            AddObject(x - 1, y - 1, object);
        }
    }

    if (deltay > threshold) {
        AddObject(x, y + 1, object);
    }
    if (-deltay > threshold) {
        AddObject(x, y - 1, object);
    }
}
