#include "../include/yds_error_handler.h"

#include "../include/yds_error_system.h"

ysErrorHandler::ysErrorHandler() {
    /* void */
}

ysErrorHandler::~ysErrorHandler() {
    /* void */
}

void ysErrorHandler::OnError(ysError error, unsigned int line, ysObject *object, const char *file) {
    (void)error;
    (void)line;
    (void)object;
    (void)file;
}
