#include "../include/yds_geometry_export_file.h"

#include <math.h>

ysGeometryExportFile::ysGeometryExportFile() : ysObject("ysGeometryExportFile") {
    /* void */
}

ysGeometryExportFile::~ysGeometryExportFile() {
    /* void */
}

ysError ysGeometryExportFile::Open(const char *fname) {
	YDS_ERROR_DECLARE("Open");

	m_file.open(fname, std::ios::binary);

	if (!m_file.is_open()) return YDS_ERROR_RETURN(ysError::YDS_COULD_NOT_OPEN_FILE);

	return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

void ysGeometryExportFile::Close() {
	if (m_file.is_open()) m_file.close();
}

void ysGeometryExportFile::FillOutputHeader(ysObjectData* object, ObjectOutputHeader* header) {
    memset(header, 0, sizeof(ObjectOutputHeader));

    strcpy_s(header->ObjectName, 64, object->m_name);
    strcpy_s(header->ObjectMaterial, 64, object->m_materialName);

    header->ModelIndex = object->m_objectInformation.ModelIndex;
    header->ParentIndex = object->m_objectInformation.ParentIndex;
    header->ParentInstanceIndex = object->m_objectInformation.ParentInstance;
    header->SkeletonIndex = object->m_objectInformation.SkeletonIndex;
    header->ObjectType = object->m_objectInformation.ObjectType;

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

    if (header->NumBones)							header->Flags |= MDF_BONES;
    if (object->m_normals.IsActive())				header->Flags |= MDF_NORMALS;
    if (object->m_tangents.IsActive())				header->Flags |= MDF_TANGENTS;
    if (object->m_objectStatistics.NumUVChannels)	header->Flags |= MDF_TEXTURE_DATA;

    float minx = FLT_MAX, miny = FLT_MAX, minz = FLT_MAX, maxx = -FLT_MAX, maxy = -FLT_MAX, maxz = -FLT_MAX;
    if (object->m_objectInformation.ObjectType == ysObjectData::TYPE_GEOMETRY) {
        for (int vert = 0; vert < object->m_objectStatistics.NumVertices; vert++) {
            ysVector3 vertex = object->m_vertices[vert];
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

		for (int i=0; i < object->m_objectStatistics.NumUVChannels; i++) {
			UVCoordinates[i] = new int [object->m_objectStatistics.NumVertices];
			memset(UVCoordinates[i], 0, sizeof(int) * object->m_objectStatistics.NumVertices);
		}

		for(int face=0; face < object->m_objectStatistics.NumFaces; face++) {
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

	for (int vert = 0; vert < object->m_objectStatistics.NumVertices; vert++) {
		ysVector3 &vertex = object->m_vertices[vert]; 

		WriteFloatToBuffer(vertex.x, &location);
		WriteFloatToBuffer(vertex.y, &location);
		WriteFloatToBuffer(vertex.z, &location);
		WriteFloatToBuffer(1.0f, &location);

		if (object->m_materialList.IsActive()) {
			//WriteIntToBuffer(object->m_materialList[vert], &location);
		}

		if (object->m_channels.IsActive()) {
			for (int channel = 0; channel < object->m_objectStatistics.NumUVChannels; channel++) {
                float u = object->m_channels[channel].m_coordinates[UVCoordinates[channel][vert]].x;
                float v = object->m_channels[channel].m_coordinates[UVCoordinates[channel][vert]].y;

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
			for (int b = 0; b < 4 /* TEMP */; b++) {
				if (b >= object->m_boneWeights[vert].m_boneIndices.GetNumObjects()) {
					WriteIntToBuffer(-1, &location);
				}
				else {
					int boneIndex = object->m_boneWeights[vert].m_boneIndices[b];
					WriteIntToBuffer(object->m_boneWeights[vert].m_boneIndices[b], &location);
				}
			}

			for (int b = 0; b < 4 /* TEMP */; b++) {
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
		for (int i=0; i < object->m_objectStatistics.NumUVChannels; i++) {
			delete [] UVCoordinates[i];
		}

		delete [] UVCoordinates;
	}

	*output = data;
	return packedSize;
}

ysError ysGeometryExportFile::WriteCustomData(void *data, int size) {
	YDS_ERROR_DECLARE("WriteCustomData");

	if (!m_file.is_open()) return YDS_ERROR_RETURN(ysError::YDS_NO_FILE);
	if (data == NULL) return YDS_ERROR_RETURN(ysError::YDS_INVALID_PARAMETER);

	m_file.write((char *)data, size);

	return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysGeometryExportFile::WriteObject(ysObjectData *object) {
	YDS_ERROR_DECLARE("WriteObject");

	if (!m_file.is_open()) return YDS_ERROR_RETURN(ysError::YDS_NO_FILE);
	if (object == NULL) return YDS_ERROR_RETURN(ysError::YDS_INVALID_PARAMETER);

	ObjectOutputHeader header;
	FillOutputHeader(object, &header);

	void *vertexData = nullptr;
	int vertexDataSize = 0;

	if (object->m_objectInformation.ObjectType == ysObjectData::TYPE_GEOMETRY) {
		vertexDataSize = PackVertexData(object, 4 /*TEMP*/, &vertexData);
		header.VertexDataSize = vertexDataSize;
	}

	m_file.write((char *)&header, sizeof(ObjectOutputHeader));

	// Geometry Data
	if (object->m_objectInformation.ObjectType == ysObjectData::TYPE_GEOMETRY) {
		m_file.write((char *)vertexData, vertexDataSize);

		for (int i  =0; i < object->m_objectStatistics.NumFaces; i++) {
			for (int facevert = 0; facevert < 3; facevert++) {
				unsigned short reduced = object->m_vertexIndexSet[i].indices[facevert];
				m_file.write((char *)&reduced, sizeof(unsigned short));
			}
		}

		// Bone Map
		for (int i = 0; i < object->m_boneIndices.GetNumObjects(); i++) {
			int *add = &object->m_boneIndices[i];
			m_file.write((char *)add, sizeof(int));
		}
	}

	// Primitive Data
	if (object->m_objectInformation.ObjectType == ysObjectData::TYPE_PLANE) {
		m_file.write((char *)&object->m_length, sizeof(float));
		m_file.write((char *)&object->m_width, sizeof(float));
	}

	return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}
