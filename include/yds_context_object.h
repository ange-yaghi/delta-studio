#ifndef YDS_CONTEXT_OBJECT_H
#define YDS_CONTEXT_OBJECT_H

#include "yds_base.h"

#include <string>

class ysContextObject : public ysObject {
public:
    enum class DeviceAPI {
        Unknown,
        DirectX10,
        DirectX11,
        OpenGL4_0,
        Vulkan
    };

public:
    ysContextObject();
    ysContextObject(const char *typeID, DeviceAPI api);
    virtual ~ysContextObject();
    DeviceAPI GetAPI() const;
    bool CheckCompatibility(const ysContextObject *object) const;

private:
    DeviceAPI m_api;
};

#endif /* YDS_CONTEXT_OBJECT_H */
