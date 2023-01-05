#include "../include/yds_tool_geometry_file.h"

ysToolGeometryFile::ysToolGeometryFile() : ysObject("ysToolGeometryFile") {
    m_header = nullptr;
    m_fileVersion = -1;

    m_lastEditor = EditorId::Undefined;
    m_compilationStatus = CompilationStatus::Undefined;
}

ysToolGeometryFile::~ysToolGeometryFile() {
    /* void */
}

ysError ysToolGeometryFile::Open(const char *fname) {
    YDS_ERROR_DECLARE("Open");

    m_file.open(fname, std::ios::binary | std::ios::in | std::ios::out);
    if (!m_file.is_open()) return YDS_ERROR_RETURN_MSG(ysError::CouldNotOpenFile, fname);

    // Read Magic Number
    unsigned int magicNumber = 0;
    m_file.read((char *)&magicNumber, sizeof(unsigned int));

    if (magicNumber != MAGIC_NUMBER) {
        m_file.close();
        return YDS_ERROR_RETURN(ysError::InvalidFileType);
    }

    // Read File Version
    int fileVersion = -1;

    m_file.read((char *)&fileVersion, sizeof(int));

    if (fileVersion < 0 || fileVersion > CURRENT_FILE_VERSION) {
        m_file.close();
        return YDS_ERROR_RETURN(ysError::UnsupportedFileVersion);
    }

    // Read Last Editor
    if (fileVersion >= 3 && fileVersion <= 5) {
        unsigned int lastEditor = 0x0;
        m_file.read((char *)&lastEditor, sizeof(unsigned int));

        if (lastEditor >= (unsigned int)EditorId::Count) {
            m_lastEditor = EditorId::Undefined;

            m_file.close();
            return YDS_ERROR_RETURN(ysError::CorruptedFile);
        }
        else {
            m_lastEditor = (EditorId)lastEditor;
        }
    }
    else {
        m_lastEditor = EditorId::Undefined;
    }

    // Read compilation status
    if (fileVersion >= 3 && fileVersion <= 5) {
        unsigned int compilationStatus = 0x0;
        m_file.read((char *)&compilationStatus, sizeof(unsigned int));

        if (compilationStatus >= (unsigned int)CompilationStatus::Count) {
            m_compilationStatus = CompilationStatus::Undefined;

            m_file.close();
            return YDS_ERROR_RETURN(ysError::CorruptedFile);
        }
        else {
            m_compilationStatus = (CompilationStatus)compilationStatus;
        }
    }
    else {
        m_compilationStatus = CompilationStatus::Undefined;
    }

    YDS_NESTED_ERROR_CALL(ReadHeader(fileVersion));

    m_fileVersion = fileVersion;

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysToolGeometryFile::ReadHeader(int fileVersion) {
    YDS_ERROR_DECLARE("ReadHeader");

    int headerSize = 0;

    if (fileVersion >= 0 && fileVersion <= 5) {
        headerSize = sizeof(HeaderVersion000);
        m_header = (void *)(malloc(headerSize));
    }

    m_file.read((char *)m_header, headerSize);

    if (!m_file) {
        free(m_header);
        m_header = NULL;

        return YDS_ERROR_RETURN(ysError::CorruptedFile);
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysToolGeometryFile::Close() {
    YDS_ERROR_DECLARE("Close");

    if (m_file.is_open()) m_file.close();

    free(m_header);

    m_header = nullptr;
    m_fileVersion = -1;

    return YDS_ERROR_RETURN(ysError::None);
}

int ysToolGeometryFile::GetObjectCount() const {
    if (m_header == NULL) return -1;

    if (m_fileVersion >= 0 && m_fileVersion <= 5) {
        HeaderVersion000 *header = (HeaderVersion000 *)m_header;
        return header->ObjectCount;
    }

    return -1;
}

bool ysToolGeometryFile::MaterialData() const {
    if (m_header == NULL) return false;

    if (m_fileVersion >= 0 && m_fileVersion <= 5) {
        HeaderVersion000 *header = (HeaderVersion000 *)m_header;
        return (header->MaterialDataCheck != 0);
    }

    return false;
}

bool ysToolGeometryFile::SmoothingData() const {
    if (m_header == nullptr) return false;

    if (m_fileVersion >= 0 && m_fileVersion <= 5) {
        HeaderVersion000 *header = (HeaderVersion000 *)m_header;
        return (header->SmoothingDataCheck != 0);
    }

    return false;
}


ysError ysToolGeometryFile::ReadObject(ysObjectData **newObject) {
    YDS_ERROR_DECLARE("ReadObject");

    if (newObject == nullptr) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    *newObject = nullptr;

    if (!m_file.is_open()) return YDS_ERROR_RETURN(ysError::NoFile);

    ysObjectData *object = new ysObjectData;
    object->Clear();

    ysError error = ysError::None;
    if (m_fileVersion == 0) {
        error = ReadObjectVersion000(object);
    }
    else if (m_fileVersion == 1) {
        error = ReadObjectVersion001(object);
    }
    else if (m_fileVersion == 2) {
        error = ReadObjectVersion002(object);
    }
    else if (m_fileVersion == 3) {
        error = ReadObjectVersion003(object);
    }
    else if (m_fileVersion == 4) {
        error = ReadObjectVersion004(object);
    }
    else if (m_fileVersion == 5) {
        error = ReadObjectVersion005(object);
    }

    if (error != ysError::None) {
        DestroyMemory();
        delete object;

        YDS_ERROR_RETURN_MANUAL();
        return error;
    }

    ReleaseMemory();
    *newObject = object;

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysToolGeometryFile::UpdateCompilationStatus(CompilationStatus status) {
    YDS_ERROR_DECLARE("UpdateCompilationStatus");

    if (m_fileVersion >= 3 && m_fileVersion <= 5) {
        std::streampos currentPosition = m_file.tellg();
        std::streampos compilationStatusOffset = 12 /* bytes */;

        m_file.seekg(compilationStatusOffset);

        m_file.write((char *)(&status), sizeof(unsigned int));

        m_file.seekg(currentPosition);
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysToolGeometryFile::ReadString(char *dest) {
    YDS_ERROR_DECLARE("ReadString");

    char c;
    int i = 0;

    do {
        m_file.read(&c, sizeof(char));
        if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

        dest[i++] = c;
    } while (c);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysToolGeometryFile::ReadObjectDataVersion000_005(ysObjectData *object) {
    YDS_ERROR_DECLARE("ReadObjectDataVersion000_005");

    if (object->m_objectInformation.objectType == ysObjectData::ObjectType::Plane) {
        m_file.read((char *)&object->m_length, sizeof(float));
        m_file.read((char *)&object->m_width, sizeof(float));
    }
    else if (object->m_objectInformation.objectType == ysObjectData::ObjectType::Geometry) {
        // ====================================================
        // Read In Vertex Data
        // ====================================================

        // Allocate Data

        int maxNumberOfBonesPerVertex = 3;
        for (int i = 0; i < object->m_objectStatistics.NumVertices; i++) {
            m_file.read((char *)&object->m_vertices.New(), sizeof(ysVector3));
            if (!m_file) {
                return YDS_ERROR_RETURN(ysError::CorruptedFile);
            }

            if (MaterialData()) {
                m_file.read((char *)&object->m_materialList.New(), sizeof(int));
            }
        }

        if (object->m_objectInformation.UsesBones != 0) {
            object->m_boneWeights.Allocate(object->m_objectStatistics.NumVertices);
            for (int i = 0; i < object->m_objectStatistics.NumVertices; i++) {
                // Bone Data
                int nBones = 0;
                m_file.read((char *)&nBones, sizeof(int));

                ysObjectData::BoneWeights *weights = &object->m_boneWeights[i];

                weights->m_boneIndices.Allocate(nBones);
                weights->m_boneWeights.Allocate(nBones);

                m_file.read((char *)weights->m_boneIndices.GetBuffer(), sizeof(int) * nBones);
                if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

                m_file.read((char *)weights->m_boneWeights.GetBuffer(), sizeof(float) * nBones);
                if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);
            }
        }

        // ====================================================
        // Read In UV Coordinate Data
        // ====================================================
        if (object->m_objectStatistics.NumUVChannels > 0) {
            object->m_channels.Allocate(object->m_objectStatistics.NumUVChannels);

            for (int i = 0; i < object->m_objectStatistics.NumUVChannels; i++) {
                int nCoordinates = 0;
                m_file.read((char *)&nCoordinates, sizeof(int));
                if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

                object->m_channels[i].m_coordinates.Allocate(nCoordinates);

                m_file.read((char *)object->m_channels[i].m_coordinates.GetBuffer(), sizeof(ysVector2) * nCoordinates);
                if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);
            }
        }

        // ====================================================
        // Read In Face Data
        // ====================================================
        object->m_vertexIndexSet.Allocate(object->m_objectStatistics.NumFaces);

        if (SmoothingData()) object->m_smoothingGroups.Allocate(object->m_objectStatistics.NumFaces);
        if (object->m_objectStatistics.NumUVChannels > 0) {
            object->m_UVIndexSets.Allocate(object->m_objectStatistics.NumUVChannels);

            for (int i = 0; i < object->m_objectStatistics.NumUVChannels; i++) {
                object->m_UVIndexSets[i].UVIndexSets.Allocate(object->m_objectStatistics.NumFaces);
            }
        }

        bool smoothingData = SmoothingData();
        for (int i = 0; i < object->m_objectStatistics.NumFaces; i++) {
            m_file.read((char *)&object->m_vertexIndexSet[i], sizeof(ysObjectData::IndexSet));
            if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

            if (smoothingData) {
                m_file.read((char *)&object->m_smoothingGroups[i], sizeof(int));
                if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);
            }

            if (object->m_objectStatistics.NumUVChannels > 0) {
                for (int channel = 0; channel < object->m_objectStatistics.NumUVChannels; channel++) {
                    m_file.read((char *)&object->m_UVIndexSets[channel].UVIndexSets[i], sizeof(ysObjectData::IndexSet));
                    if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);
                }
            }
        }

        // ====================================================
        // Read In Bone Indices
        // ====================================================
        if (object->m_objectInformation.UsesBones) {
            int nBones;
            m_file.read((char *)&nBones, sizeof(int));
            if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

            object->m_boneIndices.Allocate(nBones);
            m_file.read((char *)object->m_boneIndices.GetBuffer(), sizeof(int) * nBones);
            if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);
        }
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysToolGeometryFile::ReadObjectVersion000(ysObjectData *object) {
    YDS_ERROR_DECLARE("ReadObjectVersion000");

    ObjectInformation_000_002 objectInformation;
    m_file.read((char *)&objectInformation, sizeof(ObjectInformation_000_002));
    if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

    object->m_objectInformation.ModelIndex = objectInformation.ModelIndex;
    object->m_objectInformation.objectType = objectInformation.ObjectType;
    object->m_objectInformation.ParentIndex = objectInformation.ParentIndex;
    object->m_objectInformation.ParentInstance = -1;
    object->m_objectInformation.UsesBones = objectInformation.UsesBones;
    object->m_objectInformation.SkeletonIndex = -1;

    YDS_NESTED_ERROR_CALL(ReadString(object->m_name));

    ObjectTransformation_000_003 objectTransformation;
    m_file.read((char *)&objectTransformation, sizeof(ObjectTransformation_000_003));
    if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

    object->m_objectTransformation.OrientationEuler = objectTransformation.Orientation;
    object->m_objectTransformation.Position = objectTransformation.Position;
    object->m_objectTransformation.Scale = objectTransformation.Scale;
    object->m_objectTransformation.Orientation = ysVector4(0.0, 0.0, 0.0, 1.0);

    ObjectStatistics_000_005 objectStatistics;
    m_file.read((char *)&objectStatistics, sizeof(ObjectStatistics_000_005));
    if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

    object->m_objectStatistics.NumFaces = objectStatistics.NumFaces;
    object->m_objectStatistics.NumUVChannels = objectStatistics.NumUVChannels;
    object->m_objectStatistics.NumVertices = objectStatistics.NumVertices;

    // Version > 0x1
    object->m_flipNormals = false;
    object->m_objectInformation.ParentInstance = -1;

    YDS_NESTED_ERROR_CALL(ReadObjectDataVersion000_005(object));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysToolGeometryFile::ReadObjectVersion001(ysObjectData *object) {
    YDS_ERROR_DECLARE("ReadObjectVersion001");

    ObjectInformation_000_002 objectInformation;
    m_file.read((char *)&objectInformation, sizeof(ObjectInformation_000_002));
    if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

    object->m_objectInformation.ModelIndex = objectInformation.ModelIndex;
    object->m_objectInformation.objectType = objectInformation.ObjectType;
    object->m_objectInformation.ParentIndex = objectInformation.ParentIndex;
    object->m_objectInformation.ParentInstance = -1;
    object->m_objectInformation.UsesBones = objectInformation.UsesBones;
    object->m_objectInformation.SkeletonIndex = -1;

    YDS_NESTED_ERROR_CALL(ReadString(object->m_name));

    ObjectTransformation_000_003 objectTransformation;
    m_file.read((char *)&objectTransformation, sizeof(ObjectTransformation_000_003));
    if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

    object->m_objectTransformation.OrientationEuler = objectTransformation.Orientation;
    object->m_objectTransformation.Position = objectTransformation.Position;
    object->m_objectTransformation.Scale = objectTransformation.Scale;
    object->m_objectTransformation.Orientation = ysVector4(0.0, 0.0, 0.0, 1.0);

    ObjectStatistics_000_005 objectStatistics;
    m_file.read((char *)&objectStatistics, sizeof(ObjectStatistics_000_005));
    if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

    object->m_objectStatistics.NumFaces = objectStatistics.NumFaces;
    object->m_objectStatistics.NumUVChannels = objectStatistics.NumUVChannels;
    object->m_objectStatistics.NumVertices = objectStatistics.NumVertices;

    // ONLY ADDITION
    int flipNormals;
    m_file.read((char *)&flipNormals, sizeof(int));
    if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

    object->m_flipNormals = (flipNormals == 1);
    object->m_objectInformation.ParentInstance = -1;

    YDS_NESTED_ERROR_CALL(ReadObjectDataVersion000_005(object));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysToolGeometryFile::ReadObjectVersion002(ysObjectData *object) {
    YDS_ERROR_DECLARE("ReadObjectVersion002");

    ObjectInformation_000_002 objectInformation;
    m_file.read((char *)&objectInformation, sizeof(ObjectInformation_000_002));
    if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

    object->m_objectInformation.ModelIndex = objectInformation.ModelIndex;
    object->m_objectInformation.objectType = objectInformation.ObjectType;
    object->m_objectInformation.ParentIndex = objectInformation.ParentIndex;
    object->m_objectInformation.ParentInstance = -1;
    object->m_objectInformation.UsesBones = objectInformation.UsesBones;
    object->m_objectInformation.SkeletonIndex = -1;

    YDS_NESTED_ERROR_CALL(ReadString(object->m_name));
    YDS_NESTED_ERROR_CALL(ReadString(object->m_materialName)); // ONLY ADDITION

    ObjectTransformation_000_003 objectTransformation;
    m_file.read((char *)&objectTransformation, sizeof(ObjectTransformation_000_003));
    if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

    object->m_objectTransformation.OrientationEuler = objectTransformation.Orientation;
    object->m_objectTransformation.Position = objectTransformation.Position;
    object->m_objectTransformation.Scale = objectTransformation.Scale;
    object->m_objectTransformation.Orientation = ysVector4(0.0, 0.0, 0.0, 1.0);

    ObjectStatistics_000_005 objectStatistics;
    m_file.read((char *)&objectStatistics, sizeof(ObjectStatistics_000_005));
    if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

    object->m_objectStatistics.NumFaces = objectStatistics.NumFaces;
    object->m_objectStatistics.NumUVChannels = objectStatistics.NumUVChannels;
    object->m_objectStatistics.NumVertices = objectStatistics.NumVertices;

    int flipNormals;
    m_file.read((char *)&flipNormals, sizeof(int));
    if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

    object->m_flipNormals = (flipNormals == 1);
    object->m_objectInformation.ParentInstance = -1;

    YDS_NESTED_ERROR_CALL(ReadObjectDataVersion000_005(object));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysToolGeometryFile::ReadObjectVersion003(ysObjectData *object) {
    YDS_ERROR_DECLARE("ReadObjectVersion003");

    ObjectInformation_003_004 objectInformation;
    m_file.read((char *)&objectInformation, sizeof(ObjectInformation_003_004));
    if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

    object->m_objectInformation.ModelIndex = objectInformation.ModelIndex;
    object->m_objectInformation.objectType = objectInformation.ObjectType;
    object->m_objectInformation.ParentIndex = objectInformation.ParentIndex;
    object->m_objectInformation.ParentInstance = objectInformation.ParentInstance;
    object->m_objectInformation.UsesBones = objectInformation.UsesBones;
    object->m_objectInformation.SkeletonIndex = -1;

    YDS_NESTED_ERROR_CALL(ReadString(object->m_name));
    YDS_NESTED_ERROR_CALL(ReadString(object->m_materialName));

    ObjectTransformation_000_003 objectTransformation;
    m_file.read((char *)&objectTransformation, sizeof(ObjectTransformation_000_003));
    if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

    object->m_objectTransformation.OrientationEuler = objectTransformation.Orientation;
    object->m_objectTransformation.Position = objectTransformation.Position;
    object->m_objectTransformation.Scale = objectTransformation.Scale;
    object->m_objectTransformation.Orientation = ysVector4(0.0, 0.0, 0.0, 1.0);

    ObjectStatistics_000_005 objectStatistics;
    m_file.read((char *)&objectStatistics, sizeof(ObjectStatistics_000_005));
    if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

    object->m_objectStatistics.NumFaces = objectStatistics.NumFaces;
    object->m_objectStatistics.NumUVChannels = objectStatistics.NumUVChannels;
    object->m_objectStatistics.NumVertices = objectStatistics.NumVertices;

    int flipNormals;
    m_file.read((char *)&flipNormals, sizeof(int));
    if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

    object->m_flipNormals = (flipNormals == 1);

    YDS_NESTED_ERROR_CALL(ReadObjectDataVersion000_005(object));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysToolGeometryFile::ReadObjectVersion004(ysObjectData *object) {
    YDS_ERROR_DECLARE("ReadObjectVersion004");

    ObjectInformation_003_004 objectInformation;
    m_file.read((char *)&objectInformation, sizeof(ObjectInformation_003_004));
    if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

    object->m_objectInformation.ModelIndex = objectInformation.ModelIndex;
    object->m_objectInformation.objectType = objectInformation.ObjectType;
    object->m_objectInformation.ParentIndex = objectInformation.ParentIndex;
    object->m_objectInformation.ParentInstance = objectInformation.ParentInstance;
    object->m_objectInformation.UsesBones = objectInformation.UsesBones;
    object->m_objectInformation.SkeletonIndex = -1;

    YDS_NESTED_ERROR_CALL(ReadString(object->m_name));
    YDS_NESTED_ERROR_CALL(ReadString(object->m_materialName));

    ObjectTransformation_004_005 objectTransformation;
    m_file.read((char *)&objectTransformation, sizeof(ObjectTransformation_004_005));
    if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

    object->m_objectTransformation.OrientationEuler = objectTransformation.OrientationEuler;
    object->m_objectTransformation.Position = objectTransformation.Position;
    object->m_objectTransformation.Scale = objectTransformation.Scale;
    object->m_objectTransformation.Orientation = objectTransformation.Orientation;

    ObjectStatistics_000_005 objectStatistics;
    m_file.read((char *)&objectStatistics, sizeof(ObjectStatistics_000_005));
    if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

    object->m_objectStatistics.NumFaces = objectStatistics.NumFaces;
    object->m_objectStatistics.NumUVChannels = objectStatistics.NumUVChannels;
    object->m_objectStatistics.NumVertices = objectStatistics.NumVertices;

    int flipNormals;
    m_file.read((char *)&flipNormals, sizeof(int));
    if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

    object->m_flipNormals = (flipNormals == 1);

    YDS_NESTED_ERROR_CALL(ReadObjectDataVersion000_005(object));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysToolGeometryFile::ReadObjectVersion005(ysObjectData *object) {
    YDS_ERROR_DECLARE("ReadObjectVersion005");

    ObjectInformation_005 objectInformation;
    m_file.read((char *)&objectInformation, sizeof(ObjectInformation_005));
    if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

    object->m_objectInformation.ModelIndex = objectInformation.ModelIndex;
    object->m_objectInformation.objectType = objectInformation.ObjectType;
    object->m_objectInformation.ParentIndex = objectInformation.ParentIndex;
    object->m_objectInformation.ParentInstance = objectInformation.ParentInstance;
    object->m_objectInformation.UsesBones = objectInformation.UsesBones;
    object->m_objectInformation.SkeletonIndex = objectInformation.SkeletonIndex;

    YDS_NESTED_ERROR_CALL(ReadString(object->m_name));
    YDS_NESTED_ERROR_CALL(ReadString(object->m_materialName));

    ObjectTransformation_004_005 objectTransformation;
    m_file.read((char *)&objectTransformation, sizeof(ObjectTransformation_004_005));
    if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

    object->m_objectTransformation.OrientationEuler = objectTransformation.OrientationEuler;
    object->m_objectTransformation.Position = objectTransformation.Position;
    object->m_objectTransformation.Scale = objectTransformation.Scale;
    object->m_objectTransformation.Orientation = objectTransformation.Orientation;

    ObjectStatistics_000_005 objectStatistics;
    m_file.read((char *)&objectStatistics, sizeof(ObjectStatistics_000_005));
    if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

    object->m_objectStatistics.NumFaces = objectStatistics.NumFaces;
    object->m_objectStatistics.NumUVChannels = objectStatistics.NumUVChannels;
    object->m_objectStatistics.NumVertices = objectStatistics.NumVertices;

    int flipNormals;
    m_file.read((char *)&flipNormals, sizeof(int));
    if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

    object->m_flipNormals = (flipNormals == 1);

    YDS_NESTED_ERROR_CALL(ReadObjectDataVersion000_005(object));

    return YDS_ERROR_RETURN(ysError::None);
}

void ysToolGeometryFile::DestroyMemory() {
    int n = m_allocationTracker.GetNumObjects();
    for (int i = n - 1; i >= 0; i--) {
        m_allocationTracker.Get(i)->destroy();
        m_allocationTracker.Delete(i);
    }
}

void ysToolGeometryFile::ReleaseMemory() {
    int n = m_allocationTracker.GetNumObjects();
    for (int i = n - 1; i >= 0; i--) {
        m_allocationTracker.Delete(i);
    }
}
