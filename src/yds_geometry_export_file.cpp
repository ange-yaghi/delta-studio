#include "../include/yds_geometry_export_file.h"

#include <math.h>
#include <float.h>

ysGeometryExportFile::ysGeometryExportFile() : ysObject("ysGeometryExportFile") {
    /* void */
}

ysGeometryExportFile::~ysGeometryExportFile() {
    /* void */
}

ysError ysGeometryExportFile::Open(const char *fname) {
    YDS_ERROR_DECLARE("Open");

    m_file.open(fname, std::ios::binary);

    if (!m_file.is_open()) return YDS_ERROR_RETURN(ysError::CouldNotOpenFile);

    return YDS_ERROR_RETURN(ysError::None);
}

void ysGeometryExportFile::Close() {
    if (m_file.is_open()) m_file.close();
}

void ysGeometryExportFile::FillOutputHeader(ysObjectData* object, ObjectOutputHeader* header) {
    memset(header, 0, sizeof(ObjectOutputHeader));

    // Will be implicitly null-terminated due to memset
    strncpy(header->ObjectName, object->m_name, sizeof(header->ObjectName) - 1);
    strncpy(header->ObjectMaterial, object->m_materialName, sizeof(header->ObjectMaterial) - 1);

    header->ModelIndex = object->m_objectInformation.ModelIndex;
    header->ParentIndex = object->m_objectInformation.ParentIndex;
    header->ParentInstanceIndex = object->m_objectInformation.ParentInstance;
    header->SkeletonIndex = object->m_objectInformation.SkeletonIndex;
    header->ObjectType = static_cast<int>(object->m_objectInformation.objectType);

    header->Position = object->m_objectTransformation.Position;
    header->OrientationEuler = object->m_objectTransformation.OrientationEuler;
    header->Orientation = object->m_objectTransformation.Orientation;
    header->Scale = object->m_objectTransformation.Scale;

    header->NumUVChannels = object->m_objectStatistics.NumUVChannels;
    header->NumVertices = object->m_objectStatistics.NumVertices;
    header->NumFaces = object->m_objectStatistics.NumFaces;

    header->NumBones = object->m_boneIndices.GetNumObjects();
    header->MaxBonesPerVertex = 3; // TEMP

    header->Flags = MDF_NONE;

    if (header->NumBones)                            header->Flags |= MDF_BONES;
    if (object->m_normals.IsActive())                header->Flags |= MDF_NORMALS;
    if (object->m_tangents.IsActive())                header->Flags |= MDF_TANGENTS;
    if (object->m_objectStatistics.NumUVChannels)    header->Flags |= MDF_TEXTURE_DATA;

    float minx = FLT_MAX, miny = FLT_MAX, minz = FLT_MAX, maxx = -FLT_MAX, maxy = -FLT_MAX, maxz = -FLT_MAX;
    if (object->m_objectInformation.objectType == ysObjectData::ObjectType::Geometry) {
        for (int vert = 0; vert < object->m_objectStatistics.NumVertices; vert++) {
            const ysVector3 vertex = object->m_vertices[vert];
            if (vertex.x > maxx) maxx = vertex.x;
            if (vertex.x < minx) minx = vertex.x;
            if (vertex.y > maxy) maxy = vertex.y;
            if (vertex.y < miny) miny = vertex.y;
            if (vertex.z > maxz) maxz = vertex.z;
            if (vertex.z < minz) minz = vertex.z;
        }
    }

    header->MaxExtreme = ysVector3(maxx, maxy, maxz);
    header->MinExtreme = ysVector3(minx, miny, minz);
}

void ysGeometryExportFile::FillOutputHeader(const ysInterchangeObject *object, const VertexInfo *info, ObjectOutputHeader *header) {
    memset(header, 0, sizeof(ObjectOutputHeader));

    // Will be implicitly null-terminated due to memset
    strncpy(header->ObjectName, object->Name.c_str(), sizeof(header->ObjectName) - 1);
    strncpy(header->ObjectMaterial, object->MaterialName.c_str(), sizeof(header->ObjectMaterial) - 1);

    header->ModelIndex = object->ModelIndex;
    header->ParentIndex = object->ParentIndex;
    header->ParentInstanceIndex = object->InstanceIndex;
    header->SkeletonIndex = -1;
    header->ObjectType = static_cast<int>(object->Type);

    header->Position = object->Position;
    header->OrientationEuler = object->OrientationEuler;
    header->Orientation = object->Orientation;
    header->Scale = object->Scale;

    header->NumUVChannels = info->IncludeUVs ? info->UVChannels : 0;
    header->NumVertices = (int)object->Vertices.size();
    header->NumFaces = (int)object->VertexIndices.size();

    header->NumBones = 0;            // TODO
    header->MaxBonesPerVertex = 3;    // TODO

    header->Flags = MDF_NONE;

    if (header->NumBones > 0)                    header->Flags |= MDF_BONES;
    if (info->IncludeNormals)                    header->Flags |= MDF_NORMALS;
    if (info->IncludeTangents)                    header->Flags |= MDF_TANGENTS;
    if (header->NumUVChannels > 0)                header->Flags |= MDF_TEXTURE_DATA;

    float minx = FLT_MAX, miny = FLT_MAX, minz = FLT_MAX, maxx = -FLT_MAX, maxy = -FLT_MAX, maxz = -FLT_MAX;
    if (object->Type == ysInterchangeObject::ObjectType::Geometry) {
        for (int vert = 0; vert < header->NumVertices; vert++) {
            ysVector3 vertex = object->Vertices[vert];
            if (vertex.x > maxx) maxx = vertex.x;
            if (vertex.x < minx) minx = vertex.x;
            if (vertex.y > maxy) maxy = vertex.y;
            if (vertex.y < miny) miny = vertex.y;
            if (vertex.z > maxz) maxz = vertex.z;
            if (vertex.z < minz) minz = vertex.z;
        }
    }

    header->MaxExtreme = ysVector3(maxx, maxy, maxz);
    header->MinExtreme = ysVector3(minx, miny, minz);
}

int ysGeometryExportFile::GetVertexSize(ysInterchangeObject *object, const VertexInfo *info) {
    const int numUVChannels = info->IncludeUVs ? info->UVChannels : 0;
    const bool includeNormals = info->IncludeNormals;
    const bool includeTangents = info->IncludeTangents;

    int vertexSize = 4 * sizeof(float);
    /* TODO: bone weights */
    vertexSize += numUVChannels * (2 * sizeof(float));
    if (includeNormals) vertexSize += sizeof(float) * 4;
    if (includeTangents) vertexSize += sizeof(float) * 4;

    return vertexSize;
}

void ysGeometryExportFile::WriteIntToBuffer(int value, char **location) {
    *((int *)*location) = value;
    (*location) += sizeof(int);
}

void ysGeometryExportFile::WriteFloatToBuffer(float value, char **location) {
    *((float *)*location) = value;
    (*location) += sizeof(float);
}

int ysGeometryExportFile::PackVertexData(ysObjectData *object, int maxBonesPerVertex, void **output) {
    int packedSize = 0;

    // First Calculate Size

    int vertexSize = 4 * sizeof(float);
    //if (object->m_materialList.IsActive()) vertexSize += sizeof(int);
    if (object->m_boneWeights.IsActive() && object->m_boneWeights.GetNumObjects() > 0) vertexSize += (sizeof(int) + sizeof(float)) * maxBonesPerVertex;
    if (object->m_channels.IsActive()) vertexSize += (object->m_objectStatistics.NumUVChannels) * (2 * sizeof(float));
    if (object->m_normals.IsActive()) vertexSize += sizeof(float) * 4;
    if (object->m_tangents.IsActive()) vertexSize += sizeof(float) * 4;

    packedSize = vertexSize * object->m_objectStatistics.NumVertices;

    int **UVCoordinates = NULL;

    if (object->m_channels.IsActive()) {
        UVCoordinates = new int *[object->m_objectStatistics.NumUVChannels];
        memset(UVCoordinates, 0, sizeof(int *) * object->m_objectStatistics.NumUVChannels);

        for (int i = 0; i < object->m_objectStatistics.NumUVChannels; ++i) {
            UVCoordinates[i] = new int [object->m_objectStatistics.NumVertices];
            memset(UVCoordinates[i], 0, sizeof(int) * object->m_objectStatistics.NumVertices);
        }

        for(int face = 0; face < object->m_objectStatistics.NumFaces; ++face) {
            for (int facevert = 0; facevert < 3; facevert++) {
                for (int channel = 0; channel < object->m_objectStatistics.NumUVChannels; channel++) {
                    UVCoordinates[channel][object->m_vertexIndexSet[face].indices[facevert]] =
                        object->m_UVIndexSets[channel].UVIndexSets[face].indices[facevert];
                }
            }
        }
    }

    char *data = (char *)malloc(packedSize);
    char *location = data;

    for (int vert = 0; vert < object->m_objectStatistics.NumVertices; ++vert) {
        ysVector3 &vertex = object->m_vertices[vert];

        WriteFloatToBuffer(vertex.x, &location);
        WriteFloatToBuffer(vertex.y, &location);
        WriteFloatToBuffer(vertex.z, &location);
        WriteFloatToBuffer(1.0f, &location);

        if (object->m_materialList.IsActive()) {
            //WriteIntToBuffer(object->m_materialList[vert], &location);
        }

        if (object->m_channels.IsActive()) {
            for (int channel = 0; channel < object->m_objectStatistics.NumUVChannels; ++channel) {
                const float u = object->m_channels[channel].m_coordinates[UVCoordinates[channel][vert]].x;
                const float v = object->m_channels[channel].m_coordinates[UVCoordinates[channel][vert]].y;

                WriteFloatToBuffer(u, &location);
                WriteFloatToBuffer(v, &location);
            }
        }

        if (object->m_normals.IsActive()) {
            ysVector3 &normal = object->m_normals[vert];

            WriteFloatToBuffer(normal.x, &location);
            WriteFloatToBuffer(normal.y, &location);
            WriteFloatToBuffer(normal.z, &location);
            WriteFloatToBuffer(0.0f, &location);
        }

        if (object->m_boneWeights.IsActive() && object->m_boneIndices.GetNumObjects()) {
            for (int b = 0; b < 4 /* TEMP */; ++b) {
                if (b >= object->m_boneWeights[vert].m_boneIndices.GetNumObjects()) {
                    WriteIntToBuffer(-1, &location);
                }
                else {
                    const int boneIndex = object->m_boneWeights[vert].m_boneIndices[b];
                    WriteIntToBuffer(object->m_boneWeights[vert].m_boneIndices[b], &location);
                }
            }

            for (int b = 0; b < 4 /* TEMP */; ++b) {
                if (b >= object->m_boneWeights[vert].m_boneIndices.GetNumObjects()) {
                    WriteFloatToBuffer(0.0f, &location);
                }
                else {
                    WriteFloatToBuffer(object->m_boneWeights[vert].m_boneWeights[b], &location);
                }
            }
        }

        if (object->m_tangents.IsActive()) {
            ysVector4 &tangent = object->m_tangents[vert];

            WriteFloatToBuffer(tangent.x, &location);
            WriteFloatToBuffer(tangent.y, &location);
            WriteFloatToBuffer(tangent.z, &location);
            WriteFloatToBuffer(tangent.w, &location);
        }
    }

    if (object->m_channels.IsActive()) {
        for (int i = 0; i < object->m_objectStatistics.NumUVChannels; ++i) {
            delete[] UVCoordinates[i];
        }

        delete[] UVCoordinates;
    }

    *output = data;
    return packedSize;
}

int ysGeometryExportFile::PackVertexData(ysInterchangeObject *object, int maxBonesPerVertex, void **output, const VertexInfo *info) {
    int packedSize = 0;

    const int numUVChannels = (int)object->UVChannels.size();
    const int numRequiredUVChannels = (info->IncludeUVs) ? info->UVChannels : 0;
    const int numFaces = (int)object->VertexIndices.size();
    const int numVertices = (int)object->Vertices.size();
    const int numNormals = (int)object->Normals.size();
    const int numTangents = (int)object->Tangents.size();

    // First Calculate Size
    int vertexSize = GetVertexSize(object, info);
    packedSize = vertexSize * numVertices;

    int **UVCoordinates = nullptr;
    int *normals = nullptr;
    int *tangents = nullptr;
    if (numUVChannels > 0) {
        UVCoordinates = new int *[(int)object->UVChannels.size()];
        memset(UVCoordinates, 0, sizeof(int *) * numUVChannels);

        for (int i = 0; i < numUVChannels; ++i) {
            UVCoordinates[i] = new int[numVertices];
            memset(UVCoordinates[i], 0, sizeof(int) * numVertices);
        }

        for (int face = 0; face < numFaces; ++face) {
            for (int facevert = 0; facevert < 3; ++facevert) {
                for (int channel = 0; channel < numUVChannels; ++channel) {
                    UVCoordinates[channel][object->VertexIndices[face].indices[facevert]] =
                        object->UVIndices[channel][face].indices[facevert];
                }
            }
        }
    }

    if (info->IncludeNormals && numNormals > 0) {
        normals = new int[numVertices];

        for (int face = 0; face < numFaces; ++face) {
            for (int facevert = 0; facevert < 3; ++facevert) {
                normals[object->VertexIndices[face].indices[facevert]] =
                    object->NormalIndices[face].indices[facevert];
            }
        }
    }

    if (info->IncludeTangents && numTangents > 0) {
        tangents = new int[numVertices];

        for (int face = 0; face < numFaces; ++face) {
            for (int facevert = 0; facevert < 3; ++facevert) {
                tangents[object->VertexIndices[face].indices[facevert]] =
                    object->TangentIndices[face].indices[facevert];
            }
        }
    }

    char *data = (char *)malloc(packedSize);
    char *location = data;

    for (int vert = 0; vert < numVertices; vert++) {
        ysVector3 &vertex = object->Vertices[vert];

        WriteFloatToBuffer(vertex.x, &location);
        WriteFloatToBuffer(vertex.y, &location);
        WriteFloatToBuffer(vertex.z, &location);
        WriteFloatToBuffer(1.0f, &location);

        if (numRequiredUVChannels > 0) {
            for (int channel = 0; channel < numRequiredUVChannels; ++channel) {
                if (channel >= numUVChannels) {
                    WriteFloatToBuffer(0.0f, &location);
                    WriteFloatToBuffer(0.0f, &location);
                }
                else {
                    const float u = object->UVChannels[channel].Coordinates[UVCoordinates[channel][vert]].x;
                    const float v = object->UVChannels[channel].Coordinates[UVCoordinates[channel][vert]].y;

                    WriteFloatToBuffer(u, &location);
                    WriteFloatToBuffer(v, &location);
                }
            }
        }

        if (info->IncludeNormals) {
            const ysVector3 &normal = (numNormals > 0)
                ? object->Normals[normals[vert]]
                : ysVector3(0.0f, 0.0f, 0.0f);

            WriteFloatToBuffer(normal.x, &location);
            WriteFloatToBuffer(normal.y, &location);
            WriteFloatToBuffer(normal.z, &location);
            WriteFloatToBuffer(0.0f, &location);
        }

        /* TODO: bones */
        if (info->IncludeTangents) {
            const ysVector3 &tangent = (numTangents > 0)
                ? object->Tangents[tangents[vert]]
                : ysVector3(0.0f, 0.0f, 0.0f);

            WriteFloatToBuffer(tangent.x, &location);
            WriteFloatToBuffer(tangent.y, &location);
            WriteFloatToBuffer(tangent.z, &location);

            /* TODO: space handedness goes here */
            WriteFloatToBuffer(1.0f, &location);
        }
    }

    if (numUVChannels > 0) {
        for (int i = 0; i < numUVChannels; ++i) {
            delete[] UVCoordinates[i];
        }

        delete[] UVCoordinates;
    }

    if (numNormals > 0) delete[] normals;
    if (numTangents > 0) delete[] tangents;

    *output = data;
    return packedSize;
}

ysError ysGeometryExportFile::WriteCustomData(void *data, int size) {
    YDS_ERROR_DECLARE("WriteCustomData");

    if (!m_file.is_open()) return YDS_ERROR_RETURN(ysError::NoFile);
    if (data == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);

    m_file.write((char *)data, size);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysGeometryExportFile::WriteObject(ysObjectData *object) {
    YDS_ERROR_DECLARE("WriteObject");

    if (!m_file.is_open()) return YDS_ERROR_RETURN(ysError::NoFile);
    if (object == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);

    ObjectOutputHeader header;
    FillOutputHeader(object, &header);

    void *vertexData = nullptr;
    int vertexDataSize = 0;

    if (object->m_objectInformation.objectType == ysObjectData::ObjectType::Geometry) {
        vertexDataSize = PackVertexData(object, 4 /* TEMP */, &vertexData);
        header.VertexDataSize = vertexDataSize;
    }

    m_file.write((char *)&header, sizeof(ObjectOutputHeader));

    // Geometry Data
    if (object->m_objectInformation.objectType == ysObjectData::ObjectType::Geometry) {
        m_file.write((char *)vertexData, vertexDataSize);

        for (int i  =0; i < object->m_objectStatistics.NumFaces; ++i) {
            for (int facevert = 0; facevert < 3; facevert++) {
                unsigned short reduced = object->m_vertexIndexSet[i].indices[facevert];
                m_file.write((char *)&reduced, sizeof(unsigned short));
            }
        }

        // Bone Map
        for (int i = 0; i < object->m_boneIndices.GetNumObjects(); ++i) {
            int *add = &object->m_boneIndices[i];
            m_file.write((char *)add, sizeof(int));
        }
    }

    // Primitive Data
    if (object->m_objectInformation.objectType == ysObjectData::ObjectType::Plane) {
        m_file.write((char *)&object->m_length, sizeof(float));
        m_file.write((char *)&object->m_width, sizeof(float));
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysGeometryExportFile::WriteObject(ysInterchangeObject *object, const VertexInfo *info) {
    YDS_ERROR_DECLARE("WriteObject");

    if (!m_file.is_open()) return YDS_ERROR_RETURN(ysError::NoFile);
    if (object == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);

    VertexInfo defaultInfo;
    if (info == nullptr) {
        info = &defaultInfo;
    }

    ObjectOutputHeader header;
    FillOutputHeader(object, info, &header);

    void *vertexData = nullptr;
    int vertexDataSize = 0;

    if (object->Type == ysInterchangeObject::ObjectType::Geometry) {
        vertexDataSize = PackVertexData(object, 4 /* TEMP */, &vertexData, info);
        header.VertexDataSize = vertexDataSize;
    }

    m_file.write((char *)&header, sizeof(ObjectOutputHeader));

    // Geometry Data
    if (object->Type == ysInterchangeObject::ObjectType::Geometry) {
        m_file.write((char *)vertexData, vertexDataSize);

        for (size_t i = 0; i < object->VertexIndices.size(); ++i) {
            for (int facevert = 0; facevert < 3; ++facevert) {
                unsigned short reduced = object->VertexIndices[i].indices[facevert];
                m_file.write((char *)&reduced, sizeof(unsigned short));
            }
        }

        // TODO: Bone Map
    }
    else if (object->Type == ysInterchangeObject::ObjectType::Plane) {
        m_file.write((char *)&object->Length, sizeof(float));
        m_file.write((char *)&object->Width, sizeof(float));
    }
    else if (object->Type == ysInterchangeObject::ObjectType::Light) {
        m_file.write((char *)&object->LightInformation, sizeof(ysInterchangeObject::Light));
    }

    return YDS_ERROR_RETURN(ysError::None);
}
