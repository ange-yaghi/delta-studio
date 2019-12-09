#include "../include/SceneObject.h"
#include "../include/SceneManager.h"

#include <stdlib.h>

SceneObject::SceneObject() : ysObject("SceneObject") {
    m_parent = NULL;
    m_material = NULL;

    m_matrix = ysMath::LoadIdentity();

    m_numFaces = 0;
    m_numVertices = 0;
    m_numUVChannels = 0;
    m_vertexSize = 0;

    m_vertexBuffer = NULL;
    m_indexBuffer = NULL;

    m_baseVertex = 0;
    m_baseIndex = 0;

    m_hidden = false;
    m_validLightVolumeCache = false;
}

SceneObject::~SceneObject() {
    /* void */
}

ysMatrix SceneObject::GetFinalTransform() const {
    // Recursive call to get total transform 
    SceneObject *parent = m_manager->GetIndex(m_parent);
    return (parent != NULL) ? ysMath::MatMult(parent->GetFinalTransform(), m_matrix) : m_matrix;
}

void SceneObject::ApplyTransformation(ysMatrix &transform) {
    m_matrix = ysMath::MatMult(m_matrix, transform);
}

ysVector3 SceneObject::GetPosition() {
    ysMatrix mat = GetFinalTransform();
    mat = ysMath::Transpose(mat);

    return ysMath::GetVector3(mat.rows[3]);
}

void SceneObject::ResetTransformation() {
    m_matrix = ysMath::LoadIdentity();
}

void SceneObject::UpdateExtents() {
    ysMatrix mat = GetFinalTransform();

    float x[2], y[2], z[2];
    x[0] = ysMath::GetX(m_minPoint);
    y[0] = ysMath::GetY(m_minPoint);
    z[0] = ysMath::GetZ(m_minPoint);

    x[1] = ysMath::GetX(m_maxPoint);
    y[1] = ysMath::GetY(m_maxPoint);
    z[1] = ysMath::GetZ(m_maxPoint);

    float minx = FLT_MAX, miny = FLT_MAX, minz = FLT_MAX, maxx = -FLT_MAX, maxy = -FLT_MAX, maxz = -FLT_MAX;

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                ysVector vec = ysMath::LoadVector(x[i], y[j], z[k], 1.0f);
                vec = ysMath::MatMult(mat, vec);

                m_keyPoints[i + j * 2 + k * 4] = vec;

                float vecx = ysMath::GetX(vec);
                float vecy = ysMath::GetY(vec);
                float vecz = ysMath::GetZ(vec);

                if (vecx < minx) minx = vecx;
                if (vecx > maxx) maxx = vecx;
                if (vecy < miny) miny = vecy;
                if (vecy > maxy) maxy = vecy;
                if (vecz < minz) minz = vecz;
                if (vecz > maxz) maxz = vecz;
            }
        }
    }

    m_calcMinPoint = ysVector3(minx, miny, minz);
    m_calcMaxPoint = ysVector3(maxx, maxy, maxz);
}

bool SceneObject::Behind(ysVector &eye, ysVector &dir) {
    for (int i = 0; i < 8; i++) {
        ysVector dif = ysMath::Sub(m_keyPoints[i], eye);
        ysVector d = ysMath::Dot(dir, dif);

        if (ysMath::GetX(d) > 0) return false;
    }

    return true;
}

bool SceneObject::Intersecting(SceneObject *cmp) {
    const ysVector3 &cmpMax = cmp->m_calcMaxPoint;
    const ysVector3 &cmpMin = cmp->m_calcMinPoint;

    if (m_calcMaxPoint.x > cmpMin.x &&m_calcMinPoint.x < cmpMax.x) {
        if (m_calcMaxPoint.y > cmpMin.y &&m_calcMinPoint.y < cmpMax.y) {
            if (m_calcMaxPoint.z > cmpMin.z &&m_calcMinPoint.z < cmpMax.z) {
                return true;
            }
        }
    }

    return false;
}

bool SceneObject::Inside(SceneObject *cmp) {
    ysVector3 cmpMax = cmp->GetMax();
    ysVector3 cmpMin = cmp->GetMin();

    if (m_calcMaxPoint.x <= cmpMax.x && m_calcMinPoint.x >= cmpMin.x) {
        if (m_calcMaxPoint.y <= cmpMax.y && m_calcMinPoint.y >= cmpMin.y) {
            if (m_calcMaxPoint.z <= cmpMax.z && m_calcMinPoint.z >= cmpMin.z) {
                return true;
            }
        }
    }

    return false;
}

bool SceneObject::Intersecting(ysVector &point) {
    ysVector3 p = ysMath::GetVector3(point);

    if (m_calcMaxPoint.x > p.x &&m_calcMinPoint.x < p.x) {
        if (m_calcMaxPoint.y > p.y &&m_calcMinPoint.y < p.y) {
            if (m_calcMaxPoint.z > p.z &&m_calcMinPoint.z < p.z) {
                return true;
            }
        }
    }

    return false;
}

OcclusionVolume::OcclusionVolume() : ysObject("OcclusionVolumn") {
    m_volume = NULL;
}

OcclusionVolume::~OcclusionVolume() {
    /* void */
}

LightVolume::LightVolume() {
    /* void */
}

LightVolume::~LightVolume() {
    /* void */
}
