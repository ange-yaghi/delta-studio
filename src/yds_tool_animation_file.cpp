#include "../include/yds_tool_animation_file.h"

#include "../include/yds_time_tag_data.h"

ysToolAnimationFile::ysToolAnimationFile() : ysObject("ysToolAnimationFile") {
    m_header = NULL;
    m_fileVersion = -1;

    m_lastEditor = EditorId::UNDEFINED;
    m_compilationStatus = CompilationStatus::UNDEFINED;
}

ysToolAnimationFile::~ysToolAnimationFile() {
    /* void */
}

ysError ysToolAnimationFile::Open(const char *fname) {
    YDS_ERROR_DECLARE("Open");

    m_file.open(fname, std::ios::binary | std::ios::in | std::ios::out);

    if (!m_file.is_open()) return YDS_ERROR_RETURN_MSG(ysError::CouldNotOpenFile, fname);

    // Read magic number
    unsigned int magicNumber = 0;
    m_file.read((char *)&magicNumber, sizeof(unsigned int));

    if (magicNumber != MAGIC_NUMBER) {
        m_file.close();
        return YDS_ERROR_RETURN(ysError::InvalidFileType);
    }

    // Read File Version
    unsigned int fileVersion = 0;
    m_file.read((char *)&fileVersion, sizeof(unsigned int));

    if (fileVersion > CURRENT_FILE_VERSION) {
        m_file.close();
        return YDS_ERROR_RETURN(ysError::UnsupportedFileVersion);
    }

    // Read Last Editor
    unsigned int lastEditor = 0x0;
    m_file.read((char *)&lastEditor, sizeof(unsigned int));

    if (lastEditor >= (int)EditorId::COUNT) {
        m_lastEditor = EditorId::UNDEFINED;

        m_file.close();
        return YDS_ERROR_RETURN(ysError::CorruptedFile);
    }
    else {
        m_lastEditor = (EditorId)(lastEditor);
    }

    // Read compilation status
    unsigned int compilationStatus = 0x0;
    m_file.read((char *)&compilationStatus, sizeof(unsigned int));

    if (compilationStatus >= (int)CompilationStatus::COUNT) {
        m_compilationStatus = CompilationStatus::UNDEFINED;

        m_file.close();
        return YDS_ERROR_RETURN(ysError::CorruptedFile);
    }
    else {
        m_compilationStatus = (CompilationStatus)compilationStatus;
    }

    YDS_NESTED_ERROR_CALL(ReadHeader(fileVersion));

    m_fileVersion = fileVersion;

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysToolAnimationFile::Close() {
    YDS_ERROR_DECLARE("Close");

    if (m_file.is_open()) m_file.close();

    free(m_header);

    m_header = NULL;
    m_fileVersion = -1;

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysToolAnimationFile::ReadHeader(int fileVersion) {
    YDS_ERROR_DECLARE("ReadHeader");

    int headerSize = 0;

    if (fileVersion >= 0 && fileVersion <= 0) {
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

int ysToolAnimationFile::GetObjectCount() const {
    if (m_header == NULL) return -1;

    if (m_fileVersion >= 0 && m_fileVersion <= 0) {
        HeaderVersion000 *header = (HeaderVersion000 *)m_header;
        return header->ObjectCount;
    }

    return -1;
}

ysError ysToolAnimationFile::ReadObjectAnimation(ysObjectAnimationData **newObject) {
    YDS_ERROR_DECLARE("ReadObjectAnimation");

    if (newObject == NULL) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    *newObject = NULL;

    if (!m_file.is_open()) return YDS_ERROR_RETURN(ysError::NoFile);

    ysObjectAnimationData *object = new ysObjectAnimationData;
    object->Clear();

    ysError error;

    if (m_fileVersion == 0) {
        error = ReadObjectAnimationVersion000(object);
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

ysError ysToolAnimationFile::ReadObjectAnimationVersion000(ysObjectAnimationData *object) {
    YDS_ERROR_DECLARE("ReadObjectAnimationVersion000");

    YDS_NESTED_ERROR_CALL(ReadString(object->m_objectName));

    // Read position keys
    m_file.read((char *)&object->m_positionKeys.Type, sizeof(ysObjectAnimationData::ANIMATION_KEY));
    if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

    m_file.read((char *)&object->m_positionKeys.NumKeys, sizeof(int));
    if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

    if (object->m_positionKeys.NumKeys > 0) {
        object->m_positionKeys.Keys.Allocate(object->m_positionKeys.NumKeys);
        m_file.read((char *)object->m_positionKeys.Keys.GetBuffer(), sizeof(ysObjectAnimationData::PositionKey) * object->m_positionKeys.NumKeys);
        if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);
    }


    // Read rotation keys
    m_file.read((char *)&object->m_rotationKeys.Type, sizeof(ysObjectAnimationData::ANIMATION_KEY));
    if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

    m_file.read((char *)&object->m_rotationKeys.NumKeys, sizeof(int));
    if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

    if (object->m_positionKeys.NumKeys > 0) {
        object->m_rotationKeys.Keys.Allocate(object->m_rotationKeys.NumKeys);
        m_file.read((char *)object->m_rotationKeys.Keys.GetBuffer(), sizeof(ysObjectAnimationData::RotationKey) * object->m_rotationKeys.NumKeys);
        if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysToolAnimationFile::ReadTimeTagData(ysTimeTagData **newTimeTagData) {
    YDS_ERROR_DECLARE("ReadTimeTagData");

    if (newTimeTagData == NULL) return YDS_ERROR_RETURN(ysError::InvalidParameter);
    *newTimeTagData = NULL;

    if (!m_file.is_open()) return YDS_ERROR_RETURN(ysError::NoFile);

    ysTimeTagData *timeTagData = new ysTimeTagData;
    timeTagData->Clear();

    ysError error;

    if (m_fileVersion == 0) {
        error = ReadTimeTagDataVersion000(timeTagData);
    }

    if (error != ysError::None) {
        DestroyMemory();
        delete timeTagData;

        YDS_ERROR_RETURN_MANUAL();
        return error;
    }

    ReleaseMemory();
    *newTimeTagData = timeTagData;

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysToolAnimationFile::ReadTimeTagDataVersion000(ysTimeTagData *timeTagData) {
    YDS_ERROR_DECLARE("ReadTimeTagDataVersion000");

    m_file.read((char *)&timeTagData->m_timeTagCount, sizeof(int));
    if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);

    for (int i = 0; i < timeTagData->m_timeTagCount; i++) {
        ysTimeTagData::TimeTag *newTimeTag = &timeTagData->m_timeTags.New();

        YDS_NESTED_ERROR_CALL(ReadString(newTimeTag->Name));

        m_file.read((char *)&newTimeTag->Frame, sizeof(int));
        if (!m_file) return YDS_ERROR_RETURN(ysError::CorruptedFile);
    }

    return YDS_ERROR_RETURN(ysError::None);
}

void ysToolAnimationFile::DestroyMemory() {
    int n = m_allocationTracker.GetNumObjects();
    for (int i = n - 1; i >= 0; i--) {
        m_allocationTracker.Get(i)->destroy();
        m_allocationTracker.Delete(i);
    }
}

void ysToolAnimationFile::ReleaseMemory() {
    int n = m_allocationTracker.GetNumObjects();
    for (int i = n - 1; i >= 0; i--) {
        m_allocationTracker.Delete(i);
    }
}

ysError ysToolAnimationFile::ReadString(char *dest) {
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
