#include "../include/collision_geometry.h"

dphysics::CollisionGeometry::CollisionGeometry() : ysObject("CollisionGeometry") {
    m_parent = nullptr;
}

dphysics::CollisionGeometry::~CollisionGeometry() {
    /* void */
}

ysError dphysics::CollisionGeometry::NewBoxObject(CollisionObject **newObject) {
    YDS_ERROR_DECLARE("NewBoxObject");

    if (newObject == nullptr) return YDS_ERROR_RETURN(ysError::YDS_INVALID_PARAMETER);
    *newObject = nullptr;

    CollisionObjectSpecialized<BoxPrimitive, CollisionObject::Type::Box> *newBox =
        m_collisionObjects.NewGeneric<CollisionObjectSpecialized<BoxPrimitive, CollisionObject::Type::Box>, 16>();

    newBox->SetParent(m_parent);
    *newObject = static_cast<CollisionObject *>(newBox);

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError dphysics::CollisionGeometry::NewCircleObject(CollisionObject **newObject) {
    YDS_ERROR_DECLARE("NewCircleObject");

    if (newObject == nullptr) return YDS_ERROR_RETURN(ysError::YDS_INVALID_PARAMETER);
    *newObject = nullptr;

    CollisionObjectSpecialized<CirclePrimitive, CollisionObject::Type::Circle> *newCircle =
        m_collisionObjects.NewGeneric<CollisionObjectSpecialized<CirclePrimitive, CollisionObject::Type::Circle>, 16>();

    newCircle->SetParent(m_parent);
    *newObject = static_cast<CollisionObject *>(newCircle);

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

void dphysics::CollisionGeometry::UpdatePrimitives() {
    int nObjects = m_collisionObjects.GetNumObjects();
    for (int i = 0; i < nObjects; i++) {
        m_collisionObjects.Get(i)->ConfigurePrimitive();
    }
}
