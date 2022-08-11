#include "../include/yds_geometry_preprocessing.h"

#include <limits>
#include <stdlib.h>
#include <memory>
#include <assert.h>
#include <float.h>
#include <limits.h>

bool ysGeometryPreprocessing::ConnectedFaces(ysObjectData *object, int face1, int face2) {
    for (int i = 0; i < 3; i++) {
        for (int ii = 0; ii < 3; ii++) {
            if (object->m_vertexIndexSet[face1].indices[i] == object->m_vertexIndexSet[face2].indices[ii]) return true;
        }
    }

    return false;
}

bool ysGeometryPreprocessing::SameSmoothingGroup(ysObjectData *object, int face1, int face2) {
    if (object->m_smoothingGroups[face1] & object->m_smoothingGroups[face2]) return true;
    else if (object->m_extendedSmoothingGroups[face1] == object->m_extendedSmoothingGroups[face2]) return true;
    return false;
}

bool ysGeometryPreprocessing::SameUVGroup(ysObjectData *object, int face1, int face2, int vertex, int mapChannel) {
    int index1 = GetVertexIndex(object, face1, vertex);
    int index2 = GetVertexIndex(object, face2, vertex);

    if (object->m_UVIndexSets[mapChannel].UVIndexSets[face1].indices[index1] ==
        object->m_UVIndexSets[mapChannel].UVIndexSets[face2].indices[index2]) {
        return true;
    }
    else return false;
}

bool ysGeometryPreprocessing::IncludesVertex(ysObjectData *object, int face, int vertex) {
    for (int i = 0; i < 3; i++) {
        if (object->m_vertexIndexSet[face].indices[i] == vertex) return true;
    }

    return false;
}

void ysGeometryPreprocessing::SwapVertex(ysObjectData *object, int face, int vertex, int replacement) {
    for (int i = 0; i < 3; i++) {
        if (object->m_vertexIndexSet[face].indices[i] == vertex) {
            object->m_vertexIndexSet[face].indices[i] = replacement;
            return;
        }
    }
}

int ysGeometryPreprocessing::GetVertexIndex(ysObjectData *object, int face, int vertex) {
    for (int i = 0; i < 3; i++) {
        if (object->m_vertexIndexSet[face].indices[i] == vertex) return i;
    }

    return -1;
}

int ysGeometryPreprocessing::CreateVertexCopy(ysObjectData *object, int vertex) {
    object->m_vertices.Copy(vertex);
    if (object->m_materialList.IsActive()) object->m_materialList.Copy(vertex);
    if (object->m_boneWeights.IsActive()) object->m_boneWeights.Copy(vertex);
    if (object->m_normals.IsActive()) object->m_normals.Copy(vertex);
    if (object->m_tangents.IsActive()) object->m_tangents.Copy(vertex);

    return (object->m_vertices.GetNumObjects() - 1);
}

void ysGeometryPreprocessing::ResolveSmoothingGroupAmbiguity(ysObjectData *object) {
    unsigned int groups;

    for (int face = 0; face < object->m_objectStatistics.NumFaces; face++) {
        if (!object->m_smoothingGroups[face]) {
            groups = UINT_MAX; // ie all groups available

            for (int neighbour = 0; neighbour < object->m_objectStatistics.NumFaces; neighbour++) {
                if (face == neighbour) continue;

                if (ConnectedFaces(object, face, neighbour)) {
                    groups = groups & (~object->m_smoothingGroups[neighbour]);
                }
            }

            for (int i = 0; i < 32; i++) {
                int group = ((groups >> i) & 0x1) << i;
                if ((groups >> i) & 0x1) {
                    object->m_smoothingGroups[face] = group;
                    break;
                }
            }
        }
    }
}

void ysGeometryPreprocessing::CreateAutomaticSmoothingGroups(ysObjectData *object) {
    ysVector *tempNormals = CalculateHardNormals(object);

    object->m_extendedSmoothingGroups.Allocate(object->m_objectStatistics.NumFaces);

    // Temporary extended smoothing group designation
    for (int i = 0; i < object->m_objectStatistics.NumFaces; i++) {
        object->m_extendedSmoothingGroups[i] = object->m_objectStatistics.NumFaces + i;
    }

    const float NormalThreshold = 1.0F - 10e-5F;
    int CurrentGroup = 0;
    int count = 0;

    for (int f1 = 0; f1 < object->m_objectStatistics.NumFaces; f1++) {
        if (object->m_extendedSmoothingGroups[f1] < 0) {
            for (int f2 = 0; f2 < object->m_objectStatistics.NumFaces; f2++) {
                if (ysGeometryPreprocessing::SameSmoothingGroup(object, f1, f2)) {
                    continue;
                }

                ysVector dot = ysMath::Dot(tempNormals[f1], tempNormals[f2]);
                float similarity = ysMath::GetScalar(dot);

                if (similarity > NormalThreshold) {
                    object->m_extendedSmoothingGroups[f2] = CurrentGroup;
                    object->m_extendedSmoothingGroups[f1] = CurrentGroup;
                }
            }
        }

        CurrentGroup++;
    }

    object->m_numExtendedSmoothingGroups = CurrentGroup;
}

void ysGeometryPreprocessing::SpreadSmoothingGroup(ysObjectData *object, int face, int group, ysVector *tempNormals, int *count) {
    const float NormalThreshold = 1.0F - 10e-5F;

    (*count)++;
    if (face >= object->m_objectStatistics.NumFaces || face < 0) {
        int breakHere = 0;
    }
    object->m_extendedSmoothingGroups[face] = group;

    for (int cmpFace = 0; cmpFace < object->m_objectStatistics.NumFaces; cmpFace++) {
        if (cmpFace == face) continue;

        // Check to make sure the faces are in different smoothing groups
        if (object->m_smoothingGroups[face] & object->m_smoothingGroups[cmpFace]) continue;

        if (!ConnectedFaces(object, face, cmpFace)) continue;
        if (object->m_extendedSmoothingGroups[face] == object->m_extendedSmoothingGroups[cmpFace]) continue;

        ysVector dot = ysMath::Dot(tempNormals[face], tempNormals[cmpFace]);
        float similarity = ysMath::GetScalar(dot);

        if (similarity > NormalThreshold) {
            SpreadSmoothingGroup(object, cmpFace, group, tempNormals, count);
        }
    }
}

void ysGeometryPreprocessing::SeparateBySmoothingGroups(ysObjectData *object) {
    int faceIndex = 0;
    int vertex = 0;

    ysExpandingArray<ysExpandingArray<int, 4>> sharingCache;
    sharingCache.Allocate(object->m_objectStatistics.NumVertices);

    // Cache vertex connections
    for (int face = 0; face < object->m_objectStatistics.NumFaces; face++) {
        for (int vertIndex = 0; vertIndex < 3; vertIndex++) {
            int vert = object->m_vertexIndexSet[face].indices[vertIndex];
            sharingCache[vert].New() = face;
        }
    }

    ysExpandingArray<ysExpandingArray<int, 4>, 16> groups;
    for (int vert = 0; vert < object->m_objectStatistics.NumVertices; vert++) {
        groups.Destroy();
        groups.Preallocate(16);

        for (int face = 0; face < sharingCache[vert].GetNumObjects(); face++) {
            // Find which group this face belongs to
            int faceGroup = -1;
            for (int l = 0; l < groups.GetNumObjects(); l++) {
                for (int i = 0; i < groups[l].GetNumObjects(); i++) {
                    if (SameSmoothingGroup(object, sharingCache[vert][face], groups[l][i])) {
                        faceGroup = l;
                        break;
                    }
                }

                if (faceGroup != -1) {
                    break;
                }
            }

            if (faceGroup == -1) groups.New().New() = sharingCache[vert][face];
            else groups[faceGroup].New() = sharingCache[vert][face];
        }

        for (int l = 1; l < groups.GetNumObjects(); l++) {
            // Create a copy of the vertex
            int newVertex = ysGeometryPreprocessing::CreateVertexCopy(object, vert);

            // Adjust indicies for faces
            for (int face = 0; face < groups[l].GetNumObjects(); face++) {
                for (int facevert = 0; facevert < 3; facevert++) {
                    if (object->m_vertexIndexSet[groups[l][face]].indices[facevert] == vert) {
                        assert(groups[l][face] < object->m_objectStatistics.NumFaces);
                        assert(groups[l][face] >= 0);

                        object->m_vertexIndexSet[groups[l][face]].indices[facevert] = newVertex;
                    }
                }
            }
        }
    }

    object->m_objectStatistics.NumVertices = object->m_vertices.GetNumObjects();
}

void ysGeometryPreprocessing::SeparateByUVGroups(ysObjectData *object, int mapChannel) {
    int faceIndex = 0;
    int vertex = 0;

    ysExpandingArray<ysExpandingArray<int, 4>> sharingCache;
    sharingCache.Allocate(object->m_objectStatistics.NumVertices);

    // Cache vertex connections
    for (int face = 0; face < object->m_objectStatistics.NumFaces; face++) {
        for (int vertIndex = 0; vertIndex < 3; vertIndex++) {
            int vert = object->m_vertexIndexSet[face].indices[vertIndex];
            sharingCache[vert].New() = face;
        }
    }

    ysExpandingArray<ysExpandingArray<int, 4>, 16> leaders;

    for (int vert = 0; vert < object->m_objectStatistics.NumVertices; vert++) {
        leaders.Destroy();
        leaders.Preallocate(16);

        for (int face = 0; face < sharingCache[vert].GetNumObjects(); face++) {
            int follows = -1;
            for (int l = 0; l < leaders.GetNumObjects(); l++) {
                int f1 = sharingCache[vert][face];
                int f2 = leaders[l][0];

                if (SameUVGroup(object, f1, f2, vert, mapChannel)) {
                    break;
                    follows = l;
                }
            }

            if (follows == -1) leaders.New().New() = sharingCache[vert][face];
            else leaders[follows].New() = sharingCache[vert][face];
        }

        for (int l = 1; l < leaders.GetNumObjects(); l++) {
            // Create a copy of the vertex
            int newVertex = ysGeometryPreprocessing::CreateVertexCopy(object, vert);

            // Adjust indicies for faces
            for (int face = 0; face < leaders[l].GetNumObjects(); face++) {
                for (int facevert = 0; facevert < 3; facevert++) {
                    if (object->m_vertexIndexSet[leaders[l][face]].indices[facevert] == vert) {
                        assert(leaders[l][face] < object->m_objectStatistics.NumFaces);
                        assert(leaders[l][face] >= 0);

                        object->m_vertexIndexSet[leaders[l][face]].indices[facevert] = newVertex;
                    }
                }
            }
        }
    }

    object->m_objectStatistics.NumVertices = object->m_vertices.GetNumObjects();
}

ysVector *ysGeometryPreprocessing::CalculateHardNormals(ysObjectData *object) {
    if (object->m_hardNormalCache) return object->m_hardNormalCache;

    object->m_hardNormalCache = (ysVector *)aligned_alloc(sizeof(__m128) * object->m_objectStatistics.NumFaces, 16);
    ysVector *tempNormals = object->m_hardNormalCache;

    ysVector vert1, vert2, vert3;

    for (int face = 0; face < object->m_objectStatistics.NumFaces; face++) {
        ysVector3 v1 = object->m_vertices[object->m_vertexIndexSet[face].x];
        ysVector3 v2 = object->m_vertices[object->m_vertexIndexSet[face].y];
        ysVector3 v3 = object->m_vertices[object->m_vertexIndexSet[face].z];

        vert1 = ysMath::LoadVector(v1.x, v1.y, v1.z, 0.0);
        vert2 = ysMath::LoadVector(v2.x, v2.y, v2.z, 0.0);
        vert3 = ysMath::LoadVector(v3.x, v3.y, v3.z, 0.0);

        ysVector d1 = ysMath::Sub(vert3, vert1);
        ysVector d2 = ysMath::Sub(vert2, vert1);

        if (!object->m_flipNormals) tempNormals[face] = ysMath::Cross(d2, d1);
        else tempNormals[face] = ysMath::Cross(d1, d2);

        tempNormals[face] = ysMath::Normalize(tempNormals[face]);
    }

    return tempNormals;
}

void ysGeometryPreprocessing::CalculateNormals(ysObjectData *object) {
    object->m_normals.Allocate(object->m_objectStatistics.NumVertices);
    ysVector *tempNormals = CalculateHardNormals(object);
    ysVector *accum = (ysVector *)aligned_alloc(sizeof(__m128) * object->m_objectStatistics.NumVertices, 16);

    // Clear accum
    for (int i = 0; i < object->m_objectStatistics.NumVertices; i++) {
        accum[i] = ysMath::Constants::Zero;
    }

    for (int face = 0; face < object->m_objectStatistics.NumFaces; face++) {
        for (int facevert = 0; facevert < 3; facevert++) {
            int vert = object->m_vertexIndexSet[face].indices[facevert];
            accum[vert] = ysMath::Add(accum[vert], tempNormals[face]);
        }
    }

    // Normalize
    for (int i = 0; i < object->m_objectStatistics.NumVertices; i++) {
        ysVector normalSum = ysMath::Normalize(accum[i]);
        object->m_normals[i] = ysMath::GetVector3(normalSum);
    }

    free(accum);
}

ysVector *ysGeometryPreprocessing::CalculateHardTangents(ysObjectData *object, int mapChannel) {
    ysVector *tempTangents = (ysVector *)aligned_alloc(sizeof(__m128) * object->m_objectStatistics.NumFaces, 16);
    ysVector *hardNormals = CalculateHardNormals(object);

    ysVector vert1, vert2, vert3;

    for (int face = 0; face < object->m_objectStatistics.NumFaces; face++) {
        ysVector3 v1 = object->m_vertices[object->m_vertexIndexSet[face].x];
        ysVector3 v2 = object->m_vertices[object->m_vertexIndexSet[face].y];
        ysVector3 v3 = object->m_vertices[object->m_vertexIndexSet[face].z];

        ysVector2 u1 = object->m_channels[mapChannel].m_coordinates[object->m_UVIndexSets[mapChannel].UVIndexSets[face].indices[0]];
        ysVector2 u2 = object->m_channels[mapChannel].m_coordinates[object->m_UVIndexSets[mapChannel].UVIndexSets[face].indices[1]];
        ysVector2 u3 = object->m_channels[mapChannel].m_coordinates[object->m_UVIndexSets[mapChannel].UVIndexSets[face].indices[2]];

        vert1 = ysMath::LoadVector(v1.x, v1.y, v1.z, 0.0);
        vert2 = ysMath::LoadVector(v2.x, v2.y, v2.z, 0.0);
        vert3 = ysMath::LoadVector(v3.x, v3.y, v3.z, 0.0);

        float s1 = u2.x - u1.x;
        float t1 = u2.y - u1.y;

        float s2 = u3.x - u1.x;
        float t2 = u3.y - u1.y;

        float stDet_s = s1 * t2 - s2 * t1;
        ysVector stDet = ysMath::LoadScalar(stDet_s);
        ysVector row1 = ysMath::LoadVector(t2, -t1);
        ysVector row2 = ysMath::LoadVector(-s2, s1);

        ysVector q1 = ysMath::Sub(vert2, vert1);
        ysVector q2 = ysMath::Sub(vert3, vert1);

        ysMatrix Q_m = ysMath::LoadMatrix(q1, q2, ysMath::Constants::IdentityRow3, ysMath::Constants::IdentityRow4);
        ysMatrix st_inv_m = ysMath::LoadMatrix(row1, row2, ysMath::Constants::IdentityRow3, ysMath::Constants::IdentityRow4);

        ysMatrix TB_m = ysMath::MatMult(st_inv_m, Q_m);

        ysVector T = TB_m.rows[0];
        ysVector B = TB_m.rows[1]; // Only care about direction
        T = ysMath::Mul(T, stDet);

        // Find Handedness
        ysVector B_c = ysMath::Cross(T, hardNormals[face]);
        ysVector B_c_dot_B = ysMath::Dot(B_c, B);

        int handedness = 1;
        if (ysMath::GetX(B_c_dot_B) < 0) handedness = -1;

        // Orthogonalize
        T = ysMath::Sub(T, ysMath::Mul(ysMath::Dot(hardNormals[face], T), hardNormals[face]));

        // Add Handedness
        T = ysMath::Mask(T, ysMath::Constants::MaskOffW);
        tempTangents[face] = ysMath::Normalize(T);

        tempTangents[face] = ysMath::Add(tempTangents[face], ysMath::LoadVector(0, 0, 0, (float)handedness));
    }

    return tempTangents;
}

void ysGeometryPreprocessing::CalculateTangents(ysObjectData *object, int mapChannel) {
    ysVector *tempTangents = CalculateHardTangents(object, mapChannel);

    // Separate Faces With Discontinuous Tangents
    ysExpandingArray<int, 4> m_discontinuousFaces;

    for (int f1 = 0; f1 < object->m_objectStatistics.NumFaces; f1++) {
        m_discontinuousFaces.Clear();

        for (int f2 = 0; f2 < object->m_objectStatistics.NumFaces; f2++) {
            if (ysGeometryPreprocessing::ConnectedFaces(object, f1, f2)) {
                if (ysMath::GetX(ysMath::Dot(tempTangents[f1], tempTangents[f2])) < 0 ||
                    ((ysMath::GetW(tempTangents[f1]) > 0) != (ysMath::GetW(tempTangents[f2]) > 0))) {
                    m_discontinuousFaces.New() = f2;
                }
            }
        }

        for (int facevert = 0; facevert < 3; facevert++) {
            int vert = object->m_vertexIndexSet[f1].indices[facevert];
            int copy = -1;

            // Separate faces
            for (int f2 = 0; f2 < m_discontinuousFaces.GetNumObjects(); f2++) {
                if (ysGeometryPreprocessing::IncludesVertex(object, f2, vert)) {
                    if (copy == -1) copy = ysGeometryPreprocessing::CreateVertexCopy(object, vert);
                    SwapVertex(object, f2, vert, copy);
                }
            }
        }
    }

    // FROM HERE

    object->m_objectStatistics.NumVertices = object->m_vertices.GetNumObjects();

    // Find smoothed tangents
    object->m_tangents.Allocate(object->m_vertices.GetNumObjects());
    ysVector *accum = (ysVector *)aligned_alloc(sizeof(__m128) * object->m_objectStatistics.NumVertices, 16);

    // Clear accum
    for (int i = 0; i < object->m_objectStatistics.NumVertices; i++) {
        accum[i] = ysMath::Constants::Zero;
    }

    for (int f1 = 0; f1 < object->m_objectStatistics.NumFaces; f1++) {
        for (int facevert = 0; facevert < 3; facevert++) {
            int vert = object->m_vertexIndexSet[f1].indices[facevert];
            accum[vert] = ysMath::Add(accum[vert], tempTangents[f1]);

            // Append handedness
            accum[vert] = ysMath::Mask(accum[vert], ysMath::Constants::MaskOffW);
            accum[vert] = ysMath::Add(accum[vert], ysMath::Mask(tempTangents[f1], ysMath::Constants::MaskKeepW));
        }
    }

    // Normalize Accumulators
    for (int i = 0; i < object->m_objectStatistics.NumVertices; i++) {
        ysVector vec = ysMath::Mask(accum[i], ysMath::Constants::MaskOffW);
        vec = ysMath::Normalize(vec);
        object->m_tangents[i] = ysMath::GetVector4(vec);
        object->m_tangents[i].w = ysMath::GetW(accum[i]);
    }

    free(accum);
    free(tempTangents);
}

void ysGeometryPreprocessing::SortBoneWeights(ysObjectData *object, bool normalize, int maxBoneCount) {
    if (object->m_objectInformation.UsesBones == 1) {
        int nVertices = object->m_objectStatistics.NumVertices;

        for (int i = 0; i < nVertices; i++) {
            int nBones = object->m_boneWeights[i].m_boneWeights.GetNumObjects();

            for (int includedBone = 0; includedBone < nBones; includedBone++) {
                int heaviestBone = -1;
                float heaviestWeight = -FLT_MAX;
                for (int j = includedBone; j < nBones; j++) {
                    if (object->m_boneWeights[i].m_boneWeights[j] > heaviestWeight) {
                        heaviestWeight = object->m_boneWeights[i].m_boneWeights[j];
                        heaviestBone = object->m_boneWeights[i].m_boneIndices[j];
                    }
                }

                object->m_boneWeights[i].m_boneWeights[nBones - 1] = object->m_boneWeights[i].m_boneWeights[includedBone];
                object->m_boneWeights[i].m_boneIndices[nBones - 1] = object->m_boneWeights[i].m_boneIndices[includedBone];

                object->m_boneWeights[i].m_boneWeights[includedBone] = heaviestWeight;
                object->m_boneWeights[i].m_boneIndices[includedBone] = heaviestBone;
            }

            // Normalize Bones Weights

            if (normalize) {
                float totalWeight = 0.0f;
                for (int includedBone = 0; (includedBone < maxBoneCount) && (includedBone < nBones); includedBone++) {
                    totalWeight += object->m_boneWeights[i].m_boneWeights[includedBone];
                }

                for (int includedBone = 0; (includedBone < maxBoneCount) && (includedBone < nBones); includedBone++) {
                    object->m_boneWeights[i].m_boneWeights[includedBone] /= totalWeight;
                }
            }
        }
    }
}

void ysGeometryPreprocessing::UniformScale(ysObjectData *object, float scale) {
    int nVertices = object->m_objectStatistics.NumVertices;

    for (int i = 0; i < nVertices; i++) {
        object->m_vertices[i].x *= scale;
        object->m_vertices[i].y *= scale;
        object->m_vertices[i].z *= scale;
    }

    object->m_objectTransformation.Position.x *= scale;
    object->m_objectTransformation.Position.y *= scale;
    object->m_objectTransformation.Position.z *= scale;
}
