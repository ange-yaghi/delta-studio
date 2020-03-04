#include "../include/particle.h"
#include "../include/delta_engine.h"

#include "../include/particle_system.h"

dbasic::Particle::Particle() : ysObject("PARTICLE") {
    m_life = 0.0f;
    m_age = 0.0f;
    m_scale = 1.0f;
    m_damping = 1.0f;
    m_expansionRate = 0.0f;
    m_density = 1.0f;

    m_parent = NULL;
}

dbasic::Particle::~Particle() {
    /* void */
}

void dbasic::Particle::Render(DeltaEngine *engine, int layer) {
    ysVector finalPosition = ysMath::Add(m_position, m_parent->GetPosition());

    ysMatrix scale = ysMath::ScaleTransform(ysMath::LoadScalar(m_scale));
    ysMatrix pos = ysMath::TranslationTransform(finalPosition);
    ysMatrix final = ysMath::MatMult(scale, pos);

    float inv_density = 1.0f - m_density;
    engine->SetMultiplyColor(ysVector4(0.5f * inv_density, 0.5f * inv_density, 0.5f * inv_density, (1.0f - (m_age / m_life)) * m_density));
    engine->SetObjectTransform(final);
    engine->DrawImage(m_texture, layer);
    engine->ResetMultiplyColor();
}

void dbasic::Particle::Update(float timePassed) {
    m_velocity = ysMath::Mul(m_velocity, ysMath::LoadScalar(m_damping));
    m_position = ysMath::Add(m_position, ysMath::Mul(m_velocity, ysMath::LoadScalar(timePassed)));

    m_scale += m_expansionRate * timePassed;
    m_age += timePassed;
}
