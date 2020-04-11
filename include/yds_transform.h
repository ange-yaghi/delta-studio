#ifndef YDS_TRANSFORM_H
#define YDS_TRANSFORM_H

#include "yds_math.h"

class ysTransform {
public:
    ysTransform();
    ~ysTransform();

    void SetPosition(const ysVector &position);
    void SetOrientation(const ysQuaternion &q);

    ysMatrix GetWorldTransform();

    ysVector WorldToLocalSpace(const ysVector &p);
    ysVector LocalToWorldSpace(const ysVector &p);

    ysVector WorldToLocalDirection(const ysVector &dir);
    ysVector LocalToWorldDirection(const ysVector &dir);

    ysQuaternion GetLocalOrientation() const;
    ysQuaternion GetWorldOrientation();

    ysVector GetLocalPosition() const;
    ysVector GetWorldPosition();

    void SetParent(ysTransform *parent);
    ysTransform *GetParent() const { return m_parent; }

private:
    ysVector m_position;
    ysQuaternion m_orientation;

    ysTransform *m_parent;
};

#endif /* YDS_TRANSFORM_H */
