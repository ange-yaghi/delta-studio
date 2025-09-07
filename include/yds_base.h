#ifndef YDS_BASE_H
#define YDS_BASE_H

#include "yds_dynamic_array_element.h"

class ysObject : public ysDynamicArrayElement {
public:
    ysObject();
    ysObject(const char *typeID);
    ~ysObject() = default;

    const char* GetTypeID() const;

private:
    const char *m_typeID;
};

#endif /* YDS_BASE_H */
