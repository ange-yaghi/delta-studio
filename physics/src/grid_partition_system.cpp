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
}

dphysics::GridPartitionSystem::~GridPartitionSystem() {
    /* void */
}

void dphysics::GridPartitionSystem::Reset() {
    GridCell *gridCell;

    for (auto cell: m_gridCells) {
        gridCell = cell.second;

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
        gridCell->m_valid = true;
        gridCell->m_processed = false;
        gridCell->m_objects.Clear();
    }
}

dphysics::GridCell *dphysics::GridPartitionSystem::GetCell(int x, int y) {
    uint64_t hash = SzudzikHash(x, y);
    
    auto f = m_gridCells.find(hash);
    if (f == m_gridCells.end()) {
        GridCell *newCell = new GridCell;
        m_gridCells[hash] = newCell;

        newCell->m_x = x;
        newCell->m_y = y;
        newCell->m_forceProcess = false;
        newCell->m_valid = true;
        newCell->m_processed = false;
        newCell->m_objects.Clear();

        return newCell;
    }
    else return f->second;
}

uint64_t dphysics::GridPartitionSystem::SzudzikHash(int x, int y) {
    void *data0 = reinterpret_cast<void *>(&x);
    void *data1 = reinterpret_cast<void *>(&y);

    unsigned int *u0 = reinterpret_cast<unsigned int *>(data0);
    unsigned int *u1 = reinterpret_cast<unsigned int *>(data1);

    uint64_t a = uint64_t(*u0);
    uint64_t b = uint64_t(*u1);

    return (a >= b)
        ? a * a + a + b
        : b * b + a;
}

int dphysics::GridPartitionSystem::CalculateLoad() {
    return 0;
}

void dphysics::GridPartitionSystem::AddObject(int x, int y, RigidBody *body) {
    GridCell *gridCell = GetCell(x, y);
    gridCell->m_objects.New() = body;

    if (body->GetHint() == RigidBody::RigidBodyHint::Dynamic) {
        gridCell->m_forceProcess = true;
    }

    body->AddGridCell(x, y);
}

void dphysics::GridPartitionSystem::ProcessRigidBody(RigidBody *object) {
    object->ClearGridCells();

    ysVector pos = object->Transform.GetWorldPosition();

    float actual_x = ysMath::GetX(pos);
    float actual_y = ysMath::GetY(pos);

    int x = (actual_x >= 0)
        ? (int)(actual_x / m_gridCellSize)
        : (int)((actual_x - m_gridCellSize) / m_gridCellSize);
    int y = (actual_y >= 0)
        ? (int)(actual_y / m_gridCellSize)
        : (int)((actual_y - m_gridCellSize) / m_gridCellSize);

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
