#ifndef APTSIM_SCENE_OBJECT_H
#define APTSIM_SCENE_OBJECT_H

#include "../../../include/yds_math.h"
#include "../../../include/yds_base.h"

#include "../../../include/yds_gpu_buffer.h"
#include "../../../include/yds_expanding_array.h"

#include "Material.h"

class OcclusionVolume;
class LightVolume;
class ApartmentSimulator;
class SceneManager;

class SceneObject : public ysObject {
    friend ApartmentSimulator;
    friend SceneManager;

public:
    SceneObject();
    ~SceneObject();

    int GetVertexCount() const { return m_numVertices; }

    ysMatrix GetLocalTransform() const { return m_matrix; }
    ysMatrix GetFinalTransform() const;

    void ApplyTransformation(ysMatrix &transform);
    void ResetTransformation();

    ysVector3 GetPosition();

    void UpdateExtents();
    inline const ysVector3 &GetMax() { return m_calcMaxPoint; }
    inline const ysVector3 &GetMin() { return m_calcMinPoint; }

    bool Intersecting(SceneObject *cmp);
    bool Intersecting(ysVector &point);
    bool Inside(SceneObject *cmp);
    bool Behind(ysVector &eye, ysVector &dir);

    int GetBaseVertex() const { return m_baseVertex; }
    int GetBaseIndex() const { return m_baseIndex; }

    void SetMaterial(Material *material) { m_material = material; }

    ysExpandingArray<LightVolume *, 4> m_lightVolumeCache;
    bool m_validLightVolumeCache;

    bool m_hidden;

protected:
    char m_name[64];

    int m_parent;
    Material *m_material;

    ysMatrix m_matrix;
    ysVector m_maxPoint;
    ysVector m_minPoint;

    ysVector m_keyPoints[8];

    ysVector3 m_calcMaxPoint;
    ysVector3 m_calcMinPoint;

    // Geometry
    ysGPUBuffer *m_vertexBuffer;
    ysGPUBuffer *m_indexBuffer;

    int m_baseVertex;
    int m_baseIndex;

    int m_numFaces;
    int m_numVertices;
    int m_numUVChannels;

    int m_vertexSize;

    // Built In Types
    float m_width;
    float m_length;
    float m_height;

    SceneManager *m_manager;
};

class OcclusionVolume : public ysObject {
public:
    OcclusionVolume();
    ~OcclusionVolume();

    SceneObject *m_volume;
    ysExpandingArray<SceneObject *, 4> m_exclusions;
};

class LightVolume : public ysObject {
public:
    LightVolume();
    ~LightVolume();

    ysExpandingArray<SceneObject *, 4> m_volumes;

    int m_intermediate;
};

#endif /* APTSIM_SCENE_OBJECT_H */
