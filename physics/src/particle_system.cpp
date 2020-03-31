#include "../include/particle_system.h"

dphysics::ParticleSystem::ParticleSystem() {
    m_rate = 0.0f;
    m_source = ysMath::Constants::Zero;
    m_direction = ysMath::Constants::Zero;

    m_engine = nullptr;
    m_layer = -1;
    m_texture = nullptr;
}

dphysics::ParticleSystem::~ParticleSystem() {
    /* void */
}

void dphysics::ParticleSystem::Update(float timeStep) {
    // Phase I: Create new particles
    float expected = m_rate * timeStep;
    int n = (rand() % (100)) < 100; // Get a random number centered on the mean

    for (int i = 0; i < n; i++) {
        Particle *newParticle = m_particles.NewGeneric<Particle, 16>();
        newParticle->m_age = 0.0f;
        newParticle->m_life = ((rand() % 100) / 100.0f) + 5.0f;
        newParticle->m_texture = m_texture;
        newParticle->m_damping = 0.98f;
        newParticle->m_scale = 0.0f;
        newParticle->m_expansionRate = (rand() % 100) / 100.0f;
        newParticle->SetParent(this);
        newParticle->m_velocity = ysMath::LoadVector(float((rand() % 200) - 100), float((rand() % 200) - 100), 0.0f);
        newParticle->m_position = ysMath::Constants::Zero;
        newParticle->m_density = ((rand() % 200 - 100) / 800.0f) + 0.2f;
    }

    // Phase II: Delete dead particles
    int n_particles = GetParticleCount();
    for (int i = 0; i < n_particles; i++) {
        if (m_particles.Get(i)->GetAge() > m_particles.Get(i)->GetLife()) {
            m_particles.Delete(i);
            n_particles--;
            i--;
        }
    }

    // Phase III: Update all particles
    for (int i = 0; i < n_particles; i++) {
        m_particles.Get(i)->Update(timeStep);
    }
}
