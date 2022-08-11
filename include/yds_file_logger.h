#ifndef YDS_FILE_LOGGER_H
#define YDS_FILE_LOGGER_H

#include "yds_logger_output.h"

#include <fstream>
#include <string_view>

class ysFileLogger : public ysLoggerOutput
{

public:

    ysFileLogger();
    ~ysFileLogger();

    // Set the destination filename
    void OpenFile(const char *fname);

protected:

    // Virtual functions
    virtual void Initialize();
    virtual void Close();
    virtual void Write(std::string_view data, int padding = 0);

    // File name
    char m_fname[256];

    // File stream object
    std::fstream m_stream;

};

#endif