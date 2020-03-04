#ifndef YDS_INTERCHANGE_OBJECT_H
#define YDS_INTERCHANGE_OBJECT_H

#include "yds_math.h"

#include <vector>
#include <string>

struct ysInterchangeObject {
public:
    struct UVChannel {
        std::vector<ysVector2> Coordinates;
    };

    struct IndexSet {
        union {
            struct {
                int x;
                int y;
                int z;
            };

            int indices[3];
        };
    };

    std::string Name;
    std::string MaterialName;
    int ModelIndex;
    int ParentIndex;

    ysVector3 Position;
    ysVector3 OrientationEuler;
    ysVector4 Orientation;
    ysVector3 Scale;

    std::vector<ysVector3> Vertices;
    std::vector<UVChannel> UVChannels;
    std::vector<ysVector3> Normals;
    std::vector<ysVector3> Tangents;

    std::vector<IndexSet> VertexIndices;
    std::vector<IndexSet> NormalIndices;
    std::vector<std::vector<IndexSet>> UVIndices;
    std::vector<IndexSet> TangentIndices;

    bool Validate() const;

    void RipByIndexSet(std::vector<IndexSet> &indices);
    void RipByNormals();
    void RipByTangents();
    void RipByUVs();
};

#endif /* YDS_INTERCHANGE_OBJECT_H */
