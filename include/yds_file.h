#ifndef YDS_FILE_H
#define YDS_FILE_H

#include "yds_base.h"
#include "yds_error_codes.h"

#include <iostream>
#include <fstream>

class ysFile : ysObject {
public:
    static constexpr size_t MAX_FILE_NAME_LENGTH = 256;

    enum FILE_MODE {
        // Defaults
        FILE_WRITE = 0x000000,
        FILE_TEXT = 0x000000,

        FILE_READ = 0x000001,
        FILE_BINARY = 0x000002,
    };

public:
    ysFile();
    ysFile(const wchar_t *fname);
    ~ysFile() = default;

    ysError OpenFile(const wchar_t *fname, unsigned int filemode = FILE_READ);
    void CloseFile();

    int GetFileLength();
    void ReadFileToBuffer(char *buffer);

    bool IsOpen() const;

protected:
    wchar_t m_name[MAX_FILE_NAME_LENGTH];
    std::fstream m_file;
};

#endif /* YDS_FILE_H */
