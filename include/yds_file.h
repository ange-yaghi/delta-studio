#ifndef YDS_FILE_H
#define YDS_FILE_H

#include "yds_base.h"

#include <iostream>
#include <fstream>
#include <string>

class ysFile : ysObject {
public:

    enum FILE_MODE {
        // Defaults
        FILE_WRITE = 0x000000,
        FILE_TEXT = 0x000000,

        FILE_READ = 0x000001,
        FILE_BINARY = 0x000002,
    };

public:
    ysFile();
    ysFile(std::string fname);
    ~ysFile();

    ysError OpenFile(std::string fname, unsigned int filemode = FILE_READ);
    void CloseFile();

    int GetFileLength();
    void ReadFileToBuffer(char *buffer);

    bool IsOpen() { return m_file.is_open(); }

protected:
    std::string m_name;
    unsigned int m_filemode;

    std::fstream m_file;
};

#endif /* YDS_FILE_H */
