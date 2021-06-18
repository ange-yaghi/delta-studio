#include "../include/yds_interchange_file_0_1.h"

ysInterchangeFile0_1::ysInterchangeFile0_1() {
    m_majorVersion = 0;
    m_minorVersion = 0;

    m_toolId = -1;
    m_compilationStatus = false;
    m_objectCount = 0;
}

ysInterchangeFile0_1::~ysInterchangeFile0_1() {
    /* void */
}

ysError ysInterchangeFile0_1::Open(const char *fname) {
    YDS_ERROR_DECLARE("Open");

    m_file.open(fname, std::ios::binary | std::ios::in | std::ios::out);
    if (!m_file.is_open()) return YDS_ERROR_RETURN_MSG(ysError::CouldNotOpenFile, fname);

    IdHeader idHeader;
    m_file.read((char *)&idHeader, sizeof(IdHeader));

    if (idHeader.MagicNumber != MAGIC_NUMBER) {
        m_file.close();
        return YDS_ERROR_RETURN(ysError::InvalidFileType);
    }

    if (idHeader.MinorVersion != MINOR_VERSION || idHeader.MajorVersion != MAJOR_VERSION) {
        m_file.close();
        return YDS_ERROR_RETURN(ysError::UnsupportedFileVersion);
    }

    m_compilationStatus = idHeader.CompilationStatus == 0x0;
    m_toolId = idHeader.EditorId;

    SceneHeader sceneHeader;
    m_file.read((char *)&sceneHeader, sizeof(SceneHeader));

    m_objectCount = sceneHeader.ObjectCount;

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysInterchangeFile0_1::ReadObject(ysInterchangeObject *object) {
    YDS_ERROR_DECLARE("ReadObject");

    ObjectInformation info;
    m_file.read((char *)&info, sizeof(ObjectInformation));

    object->MaterialName = info.MaterialName;
    object->Name = info.Name;
    object->ModelIndex = info.ModelIndex;
    object->ParentIndex = info.ParentIndex;
    object->InstanceIndex = info.InstanceIndex;
    object->Type = InterpretType(info.ObjectType);

    const bool isInstance = object->InstanceIndex != -1;

    ObjectTransformation t;
    m_file.read((char *)&t, sizeof(ObjectTransformation));

    object->Orientation = t.Orientation;
    object->OrientationEuler = t.OrientationEuler;
    object->Position = t.Position;
    object->Scale = t.Scale;

    if (object->Type == ysInterchangeObject::ObjectType::Geometry) {
        GeometryInformation geometryInfo;
        m_file.read((char *)&geometryInfo, sizeof(GeometryInformation));

        const int normalCount = geometryInfo.NormalCount;
        const int uvChannelCount = geometryInfo.UVChannelCount;
        const int vertexCount = geometryInfo.VertexCount;
        const int faceCount = geometryInfo.FaceCount;
        const int tangentCount = geometryInfo.TangentCount;

        for (int i = 0; i < vertexCount; ++i) {
            ysVector3 v;
            m_file.read((char *)&v, sizeof(ysVector3));
            object->Vertices.push_back(v);
        }

        for (int i = 0; i < uvChannelCount; ++i) {
            UVChannel channel;
            m_file.read((char *)&channel, sizeof(UVChannel));

            ysInterchangeObject::UVChannel objectChannel;
            for (unsigned int j = 0; j < channel.UVCount; ++j) {
                ysVector2 uv;
                m_file.read((char *)&uv, sizeof(ysVector2));
                objectChannel.Coordinates.push_back(uv);
            }

            object->UVChannels.push_back(objectChannel);
        }

        for (int i = 0; i < normalCount; ++i) {
            ysVector3 v;
            m_file.read((char *)&v, sizeof(ysVector3));
            object->Normals.push_back(v);
        }

        for (int i = 0; i < tangentCount; ++i) {
            ysVector3 v;
            m_file.read((char *)&v, sizeof(ysVector3));
            object->Tangents.push_back(v);
        }

        for (int i = 0; i < uvChannelCount; ++i) {
            object->UVIndices.push_back({});
        }

        for (int i = 0; i < faceCount; ++i) {
            IndexSet vi, ni, ti;
            m_file.read((char *)&vi, sizeof(IndexSet));
            m_file.read((char *)&ni, sizeof(IndexSet));
            m_file.read((char *)&ti, sizeof(IndexSet));

            object->VertexIndices.push_back({ vi.u, vi.v, vi.w });
            object->NormalIndices.push_back({ ni.u, ni.v, ni.w });
            object->TangentIndices.push_back({ ti.u, ti.v, ti.w });

            for (int j = 0; j < uvChannelCount; ++j) {
                IndexSet u;
                m_file.read((char *)&u, sizeof(IndexSet));
                object->UVIndices[j].push_back({ u.u, u.v, u.w });
            }
        }
    }
    else if (object->Type == ysInterchangeObject::ObjectType::Light) {
        LightInformation lightInformation;
        m_file.read((char *)&lightInformation, sizeof(LightInformation));

        object->LightInformation.Color = lightInformation.Color;
        object->LightInformation.CutoffDistance = lightInformation.CutoffDistance;
        object->LightInformation.Distance = lightInformation.Distance;
        object->LightInformation.Intensity = lightInformation.Intensity;
        object->LightInformation.LightType = lightInformation.LightType;
        object->LightInformation.SpotAngularSize = lightInformation.SpotAngularSize;
        object->LightInformation.SpotFade = lightInformation.SpotFade;
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysInterchangeObject::ObjectType ysInterchangeFile0_1::InterpretType(int i) {
    switch (i) {
    case 0x00: return ysInterchangeObject::ObjectType::Geometry;
    case 0x01: return ysInterchangeObject::ObjectType::Bone;
    case 0x02: return ysInterchangeObject::ObjectType::Group;
    case 0x03: return ysInterchangeObject::ObjectType::Plane;
    case 0x04: return ysInterchangeObject::ObjectType::Instance;
    case 0x05: return ysInterchangeObject::ObjectType::Empty;
    case 0x06: return ysInterchangeObject::ObjectType::Armature;
    case 0x07: return ysInterchangeObject::ObjectType::Light;
    default: return ysInterchangeObject::ObjectType::Undefined;
    }
}

ysError ysInterchangeFile0_1::Close() {
    YDS_ERROR_DECLARE("Close");

    if (m_file.is_open()) m_file.close();

    m_majorVersion = -1;
    m_minorVersion = -1;

    return YDS_ERROR_RETURN(ysError::None);
}
