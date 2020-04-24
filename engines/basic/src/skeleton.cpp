#include "../include/skeleton.h"

dbasic::Skeleton::Skeleton() : ysObject("Skeleton") {
    m_rootBone = NULL;
}

dbasic::Skeleton::~Skeleton() {
    /* void */
}

dbasic::Bone *dbasic::Skeleton::NewBone() {
    Bone *newBone = m_bones.NewGeneric<Bone, 16>();
    newBone->SetSkeleton(this);

    return newBone;
}

dbasic::Bone *dbasic::Skeleton::GetBone(int index) {
    return m_bones.Get(index);
}

dbasic::Bone *dbasic::Skeleton::FindBone(const char *boneName) {
    int nBones = m_bones.GetNumObjects();

    for (int i = 0; i < nBones; i++) {
        if (strcmp(m_bones.Get(i)->GetName(), boneName) == 0) {
            return m_bones.Get(i);
        }
    }

    return NULL;
}

void dbasic::Skeleton::Update() {
    int nBones = m_bones.GetNumObjects();
    for (int i = 0; i < nBones; i++) {
        Bone *bone = m_bones.Get(i);
        //bone->RigidBody.UpdateDerivedData();
    }
}
