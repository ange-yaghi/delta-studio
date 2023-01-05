#include "../include/yds_object_data.h"

#include <stdlib.h>

ysObjectData::ysObjectData() {
    Clear();
}

ysObjectData::~ysObjectData() {
    /* void */
}

void ysObjectData::Clear() {
    m_name[0] = '\0';
    m_materialName[0] = '\0';

    m_objectInformation.ModelIndex = -1;
    m_objectInformation.ParentIndex = -1;
    m_objectInformation.ParentInstance = -1;
    m_objectInformation.objectType = ObjectType::Undefined;
    m_objectInformation.UsesBones = 0;

    m_objectStatistics.NumUVChannels = 0;
    m_objectStatistics.NumVertices = 0;
    m_objectStatistics.NumFaces = 0;

    m_vertices.Clear();
    m_materialList.Clear();
    m_boneWeights.Clear();

    m_channels.Clear();

    m_vertexIndexSet.Clear();
    m_smoothingGroups.Clear();
    m_extendedSmoothingGroups.Clear();
    m_numExtendedSmoothingGroups = 0;
    m_UVIndexSets.Clear();

    m_boneIndices.Clear();

    m_normals.Clear();
    m_tangents.Clear();

    m_flipNormals = false;

    m_hardNormalCache = nullptr;
}
