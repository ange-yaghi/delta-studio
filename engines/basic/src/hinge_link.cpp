#include "../include/hinge_link.h"
#include "../include/rigid_body.h"

#include "../include/delta_engine.h"

dbasic::HingeLink::HingeLink() {
    m_relativePos1 = ysMath::Constants::Zero;
    m_relativePos2 = ysMath::Constants::Zero;
}

dbasic::HingeLink::~HingeLink() {
    /* void */
}

int dbasic::HingeLink::GenerateCollisions(Collision *collisionArray) {
    ysVector actualPosition1 = m_body1->GetGlobalSpace(m_relativePos1);
    ysVector actualPosition2 = m_body2->GetGlobalSpace(m_relativePos2);

    ysVector delta = ysMath::Sub(actualPosition2, actualPosition1);
    ysVector length = ysMath::Magnitude(delta);
    ysVector normal = ysMath::Div(delta, length);

    float penetration = ysMath::GetScalar(length);

    Collision &retcol = collisionArray[0];
    retcol.m_body1 = m_body1;
    retcol.m_body2 = m_body2;
    retcol.m_normal = normal;
    retcol.m_penetration = penetration;
    retcol.m_position = actualPosition1;

    return 1;
}

void dbasic::HingeLink::DrawDebug(DeltaEngine *engine, int layer) {
    int red[3] = { 255, 0, 0 };
    int blue[3] = { 0, 0, 255 };

    ysVector actualPosition1 = m_body1->GetGlobalSpace(m_relativePos1);
    ysVector actualPosition2 = m_body2->GetGlobalSpace(m_relativePos2);

    engine->SetObjectTransform(ysMath::TranslationTransform(actualPosition2));
    engine->DrawBox(red, 20, 20, layer);

    engine->SetObjectTransform(ysMath::TranslationTransform(actualPosition1));
    engine->DrawBox(blue, 20, 20, layer);
}
