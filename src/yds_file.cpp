#include "../include/yds_file.h"

#if !defined(_WIN64)
    #include "safe_lib.h"
#endif

ysFile::ysFile() : ysObject("FILE") {
    m_name[0] = '\0';
}

ysFile::ysFile(const wchar_t *fname) : ysObject("FILE") {
    OpenFile(fname);
}

ysFile::~ysFile() {
    if (m_file.is_open()) {
        m_file.close();
    }
}

ysError ysFile::OpenFile(const wchar_t *fname, unsigned int filemode) {
    YDS_ERROR_DECLARE("OpenFile");

    wcscpy_s(m_name, MAX_FILE_NAME_LENGTH, fname);

    unsigned int openMode=0;
    if (filemode & FILE_READ) openMode |= std::ios::in;
    else openMode |= std::ios::out;

    if (filemode & FILE_BINARY) openMode |= std::ios::binary;

    m_file.open(fname, openMode);
    if (!m_file.is_open()) {
        return YDS_ERROR_RETURN(ysError::CouldNotOpenFile);
    }

    return YDS_ERROR_RETURN(ysError::None);
}

void ysFile::CloseFile() {
    if (m_file.is_open()) {
        m_file.close();
    }
}

int ysFile::GetFileLength() {
    std::streampos fsize = 0;

    m_file.seekg(0, std::ios::beg);
    fsize = m_file.tellg();
    m_file.seekg(0, std::ios::end);
    fsize = m_file.tellg() - fsize;
    m_file.seekg(0, std::ios::beg);

    return (int)fsize;
}

void ysFile::ReadFileToBuffer(char *buffer) {
    int fileLength = GetFileLength();

    m_file.read(buffer, fileLength);
}
