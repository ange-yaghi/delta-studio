#include "../include/bone.h"

dbasic::Bone::Bone() {
    m_parent = NULL;
    m_assetID = -1;
}

dbasic::Bone::~Bone() {
    /* void */
}

void dbasic::Bone::SetParent(Bone *parent) {
    m_parent = parent;

    Transform.SetParent(&parent->Transform);
}

void dbasic::Bone::SetReferenceTransform(const ysMatrix &m) {
    m_referenceTransform = m;
    m_inverseReferenceTransform = ysMath::OrthogonalInverse(m);

    ysMatrix test = ysMath::MatMult(m_inverseReferenceTransform, m_referenceTransform);
    int a = 0;
}

ysMatrix dbasic::Bone::GetSkinMatrix() {
    ysMatrix ref = m_referenceTransform;
    ysMatrix invRef = m_inverseReferenceTransform;

    ysMatrix test = ysMath::MatMult(invRef, ref);

    ysMatrix trans = Transform.GetWorldTransform();

    ysMatrix skin = ysMath::MatMult(trans, invRef);

    return skin;
}
