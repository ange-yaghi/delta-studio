#include "../include/yds_interchange_object.h"

#include <map>
#include <vector>

bool ysInterchangeObject::Validate() const {
    if (ModelIndex < 0) return false;

    const int vertexCount = (int)Vertices.size();
    const int normalCount = (int)Normals.size();
    const int tangentCount = (int)Tangents.size();
    const int channelCount = (int)UVChannels.size();

    for (const IndexSet &s : VertexIndices) {
        if (s.x >= vertexCount || s.y >= vertexCount || s.z >= vertexCount) return false;
    }

    for (const IndexSet &s : NormalIndices) {
        if (s.x >= normalCount || s.y >= normalCount || s.z >= normalCount) return false;
    }

    for (const IndexSet &s : TangentIndices) {
        if (s.x >= tangentCount || s.y >= tangentCount || s.z >= tangentCount) return false;
    }

    for (int i = 0; i < channelCount; ++i) {
        int channelSize = (int)UVChannels[i].Coordinates.size();
        for (const IndexSet &s: UVIndices[i]) {
            if (s.x >= channelSize || s.y >= channelSize || s.z >= channelSize) return false;
        }
    }

    return true;
}

void ysInterchangeObject::RipByIndexSet(std::vector<IndexSet> &indices) {
    int N = (int)Vertices.size();
    const int faces = (int)VertexIndices.size();

    std::vector<int> vertexToIndex(N, -1);
    std::vector<int> newVertex(N, -1);
    for (int i = 0; i < faces; ++i) {
        IndexSet &v = VertexIndices[i];
        IndexSet &r = indices[i];

        for (int j = 0; j < 3; ++j) {
            if (vertexToIndex[v.indices[j]] == -1) vertexToIndex[v.indices[j]] = r.indices[j];
            else if (vertexToIndex[v.indices[j]] != r.indices[j]) {
                int next = newVertex[v.indices[j]];
                int last = v.indices[j];
                while (next != -1) {
                    if (vertexToIndex[next] == r.indices[j]) break;
                    last = next;
                    next = newVertex[next];
                }

                int newVertexId = next;
                if (newVertexId == -1) {
                    // Copy the vertex
                    Vertices.push_back(Vertices[v.indices[j]]);

                    vertexToIndex.push_back(r.indices[j]);
                    newVertex.push_back(-1);
                    newVertexId = N++;
                }

                newVertex[last] = newVertexId;
                v.indices[j] = newVertexId;
            }
        }
    }
}

void ysInterchangeObject::RipByNormals() {
    RipByIndexSet(NormalIndices);
}

void ysInterchangeObject::RipByTangents() {
    RipByIndexSet(TangentIndices);
}

void ysInterchangeObject::RipByUVs() {
    int channelCount = (int)UVChannels.size();
    for (int i = 0; i < channelCount; ++i) {
        RipByIndexSet(UVIndices[i]);
    }
}

void ysInterchangeObject::UniformScale(float scale) {
    for (ysVector3 &vert : Vertices) {
        vert.x *= scale;
        vert.y *= scale;
        vert.z *= scale;
    }

    Position.x *= scale;
    Position.y *= scale;
    Position.z *= scale;
}
