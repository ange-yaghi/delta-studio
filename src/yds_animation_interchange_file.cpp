#include "../include/yds_animation_interchange_file.h"

#include "../include/yds_animation_interchange_file_reader_0_0.h"
#include "../include/yds_animation_interchange_file_reader_0_1.h"

ysAnimationInterchangeFile::ysAnimationInterchangeFile() {
    m_reader = nullptr;
    m_majorVersion = 0x0;
    m_minorVersion = 0x0;
    m_toolId = -1;
    m_compilationStatus = false;
}

ysAnimationInterchangeFile::~ysAnimationInterchangeFile() {
    /* void */
}

ysError ysAnimationInterchangeFile::Open(const char *fname) {
    YDS_ERROR_DECLARE("Open");

    m_file.open(fname, std::ios::binary | std::ios::in | std::ios::out);
    if (!m_file.is_open()) return YDS_ERROR_RETURN_MSG(ysError::CouldNotOpenFile, fname);

    IdHeader idHeader;
    m_file.read((char *)&idHeader, sizeof(IdHeader));

    if (idHeader.MagicNumber != MAGIC_NUMBER) {
        m_file.close();
        return YDS_ERROR_RETURN(ysError::InvalidFileType);
    }

    m_reader = nullptr;
    if (idHeader.MajorVersion == 0x0) {
        if (idHeader.MinorVersion == 0x0) {
            m_reader = new ysAnimationInterchangeFileReader_0_0;
        }
        else if (idHeader.MinorVersion == 0x1) {
            m_reader = new ysAnimationInterchangeFileReader_0_1;
        }
    }

    if (m_reader == nullptr) {
        m_file.close();
        return YDS_ERROR_RETURN(ysError::UnsupportedFileVersion);
    }

    m_compilationStatus = idHeader.CompilationStatus == 0x0;
    m_toolId = idHeader.EditorId;

    m_reader->ReadHeader(m_file);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError ysAnimationInterchangeFile::Close() {
    YDS_ERROR_DECLARE("Close");

    if (m_file.is_open()) m_file.close();

    m_majorVersion = -1;
    m_minorVersion = -1;

    delete m_reader;

    return YDS_ERROR_RETURN(ysError::None);
}

unsigned int ysAnimationInterchangeFile::GetActionCount() const {
    return m_reader->GetActionCount();
}

ysError ysAnimationInterchangeFile::ReadAction(ysAnimationAction *action) {
    YDS_ERROR_DECLARE("ReadAction");

    YDS_NESTED_ERROR_CALL(m_reader->ReadAction(m_file, action));

    return YDS_ERROR_RETURN(ysError::None);
}
