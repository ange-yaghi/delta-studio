#include "../include/collision_geometry.h"

dbasic::CollisionGeometry::CollisionGeometry() : ysObject("CollisionGeometry") {
    m_parent = nullptr;
}

dbasic::CollisionGeometry::~CollisionGeometry() {
    /* void */
}

ysError dbasic::CollisionGeometry::NewBoxObject(CollisionObject **newObject) {
    YDS_ERROR_DECLARE("NewBoxObject");

    if (newObject == NULL) return YDS_ERROR_RETURN(ysError::YDS_INVALID_PARAMETER);
    *newObject = NULL;

    CollisionObjectSpecialized<BoxPrimitive, CollisionObject::COLLISION_OBJECT_TYPE_BOX> *newBox =
        m_collisionObjects.NewGeneric<CollisionObjectSpecialized<BoxPrimitive, CollisionObject::COLLISION_OBJECT_TYPE_BOX>, 16>();

    newBox->SetParent(m_parent);
    *newObject = static_cast<CollisionObject *>(newBox);

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError dbasic::CollisionGeometry::NewCircleObject(CollisionObject **newObject) {
    YDS_ERROR_DECLARE("NewCircleObject");

    if (newObject == NULL) return YDS_ERROR_RETURN(ysError::YDS_INVALID_PARAMETER);
    *newObject = NULL;

    CollisionObjectSpecialized<CirclePrimitive, CollisionObject::COLLISION_OBJECT_TYPE_CIRCLE> *newCircle =
        m_collisionObjects.NewGeneric<CollisionObjectSpecialized<CirclePrimitive, CollisionObject::COLLISION_OBJECT_TYPE_CIRCLE>, 16>();

    newCircle->SetParent(m_parent);
    *newObject = static_cast<CollisionObject *>(newCircle);

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

void dbasic::CollisionGeometry::UpdatePrimitives() {
    int nObjects = m_collisionObjects.GetNumObjects();
    for (int i = 0; i < nObjects; i++) {
        m_collisionObjects.Get(i)->ConfigurePrimitive();
    }
}
