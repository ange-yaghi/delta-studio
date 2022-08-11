#ifndef YDS_FILE_LOGGER_H
#define YDS_FILE_LOGGER_H

#include "yds_logger_output.h"

#include <fstream>
#include <string>
#include <string_view>

class ysFileLogger : public ysLoggerOutput
{

public:

    ysFileLogger();
    ~ysFileLogger();

    // Set the destination filename
    void OpenFile(std::string fname);

protected:

    // Virtual functions
    virtual void Initialize();
    virtual void Close();
    virtual void Write(std::string_view data, int padding = 0);

    // File name
    std::string m_fname;

    // File stream object
    std::fstream m_stream;

};

#endif