#ifndef YDS_ERROR_SYSTEM_H
#define YDS_ERROR_SYSTEM_H

#include "yds_base.h"
#include "yds_error_codes.h"

#include <mutex>

class ysErrorHandler;
class ysErrorSystem : public ysObject {
protected:
    ysErrorSystem();
    ~ysErrorSystem();

    static ysErrorSystem *g_instance;

public:
    static ysErrorSystem *GetInstance();
    static void Destroy();

    static const int MAX_STACK_LEVEL = 256;

    ysError RaiseError(ysError error, unsigned int line, ysObject *object, const char *file, const char *msg, bool affectStack = true);

    template<typename T_ErrorHandler>
    ysError AttachErrorHandler(T_ErrorHandler **handler) {
        if (handler == nullptr) return ysError::InvalidParameter;

        // Create the new handler
        *handler = m_errorHandlers.NewGeneric<T_ErrorHandler>();

        return ysError::None;
    }

    ysError DetachErrorHandler(ysErrorHandler *handler);

protected:
    ysDynamicArray<ysErrorHandler, 4> m_errorHandlers;

private:
    std::mutex m_lock;
};

#define _YDS_WIDE(_String) L ## _String
#define YDS_WIDE(_String) _YDS_WIDE(_String)

#define YDS_ERROR_RETURN(error) ysErrorSystem::GetInstance()->RaiseError(error, __LINE__, this, __FILE__, "")

#define YDS_ERROR_RETURN_MANUAL()

#define YDS_ERROR_RETURN_STATIC(error) ysErrorSystem::GetInstance()->RaiseError(error, __LINE__, NULL, __FILE__, "")

#define YDS_ERROR_RAISE(error) ysErrorSystem::GetInstance()->RaiseError(error, __LINE__, this, __FILE__, "", false)

#define YDS_ERROR_RETURN_MSG(error, msg) ysErrorSystem::GetInstance()->RaiseError(error, __LINE__, this, __FILE__, msg)

#define YDS_NESTED_ERROR_CALL(call)                     \
{                                                       \
                                                        \
    ysError code = (call);                              \
    if (code != ysError::None)                          \
    {                                                   \
        return code;                                    \
    }                                                   \
                                                        \
}

#define YDS_ERROR_DECLARE(call) \
    (call)

#endif /* YDS_ERROR_SYSTEM_H */
