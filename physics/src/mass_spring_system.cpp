#include "../include/mass_spring_system.h"

#include <stdlib.h>

#define max(a ,b)            (((a) > (b)) ? (a) : (b))

// MSS Spring
dphysics::MSSSpring::MSSSpring() {
    m_length = 0.0f;
    m_constant = 0.0f;

    m_particle0 = NULL;
    m_particle1 = NULL;

    m_invertedForce = false;
}

dphysics::MSSSpring::~MSSSpring() {
    /* void */
}

void dphysics::MSSSpring::SetParticle0(MSSParticle *particle0) {
    if (particle0 == NULL) return;

    m_particle0 = particle0;
    m_particle0->RegisterSpring(this);
}

void dphysics::MSSSpring::SetParticle1(MSSParticle *particle1) {
    if (particle1 == NULL) return;

    m_particle1 = particle1;
    m_particle1->RegisterSpring(this);
}

// MSS Particle
dphysics::MSSParticle::MSSParticle() {
    m_inverseMass = 0.0f;

    m_velocity = ysMath::Constants::Zero;
    m_position = ysMath::Constants::Zero;
    m_externalAcceleration = ysMath::Constants::Zero;

    m_drag = 1.0f;

    m_radius = 0.0f;
    m_enableCollisions = false;

    m_averageVelocity = ysMath::Constants::Zero;
    m_averageVelocitySamples = 0;
}

dphysics::MSSParticle::~MSSParticle() {
    /* void */
}

ysVector dphysics::MSSParticle::CalculateAcceleration(MassSpringSystem::RK4_PASS pass, float timeDelta) {
    ysVector acceleration = ExternalAcceleration(pass);
    ysVector position = GetPosition(pass);
    ysVector velocity = GetVelocity(pass);

    int adjacentCount = m_adjacentSprings.GetNumObjects();
    ysVector diff, force;
    float ratio;

    for (int i = 0; i < adjacentCount; i++) {
        MSSSpring *spring = m_adjacentSprings[i];

        if (this != spring->GetParticle0()) {
            diff = ysMath::Sub(spring->GetParticle0()->GetPosition(pass), position);
        }
        else {
            diff = ysMath::Sub(spring->GetParticle1()->GetPosition(pass), position);
        }

        float actualLength = ysMath::GetScalar(ysMath::Magnitude(diff));
        ratio = spring->GetLength(timeDelta) / actualLength;

        float falloff = 1.0f;

        if (spring->IsInvertedForce()) {
            falloff = powf(2, spring->GetLength(timeDelta) - actualLength);
            falloff = std::fmin(falloff, 1.0f); // Clamp the value to 0.0 - 1.0
        }

        force = ysMath::Mul(ysMath::LoadScalar(spring->GetConstant() * (1.0f - ratio) * falloff), diff);
        acceleration = ysMath::Add(
            acceleration,
            ysMath::Mul(ysMath::LoadScalar(m_inverseMass), force)
        );
    }

    return acceleration;
}

ysVector dphysics::MSSParticle::ExternalAcceleration(MassSpringSystem::RK4_PASS pass) {
    ysVector velocity = GetVelocity(pass);

    float velocity_s = ysMath::GetScalar(ysMath::Magnitude(velocity));

    if (velocity_s > 0.00001f) {
        velocity = ysMath::Div(velocity, ysMath::LoadScalar(velocity_s));
    }

    ysVector viscosity = ysMath::LoadScalar(-0.5f * velocity_s * m_drag * 1.0f);
    viscosity = ysMath::LoadScalar(-0.5f * velocity_s * m_drag * 10.0f);

    ysVector drag = ysMath::Mul(viscosity, velocity);

    // Collisions
    int numCollisions = m_collidingParticles.GetNumObjects();
    ysVector avoidance = ysMath::Constants::Zero;

    for (int i = 0; i < numCollisions; i++) {
        ysVector pos1 = m_collidingParticles[i]->GetPosition(pass);
        ysVector pos2 = GetPosition(pass);
        ysVector delta = ysMath::Sub(pos2, pos1);

        ysVector dist = ysMath::Magnitude(delta);
        ysVector dist2 = ysMath::Mul(dist, dist);
        delta = ysMath::Div(delta, dist);

        float massRatio;
        float mass;
        if (GetInverseMass() <= 0.0f) massRatio = 0.0f;
        else if (m_collidingParticles[i]->GetInverseMass() <= 0.0f) massRatio = 1.0f;
        else {
            mass = 1.0f / GetInverseMass();
            massRatio = 1.0f - (mass) / (mass + (1.0f / m_collidingParticles[i]->GetInverseMass()));
        }

        float dist2_f = ysMath::GetScalar(dist2);
        dist2_f = max(dist2_f, 0.05f);

        avoidance = ysMath::Add(avoidance, ysMath::Mul(ysMath::LoadScalar(5.0f * massRatio), ysMath::Mul(ysMath::Div(ysMath::Constants::One, ysMath::LoadScalar(dist2_f)), delta)));
        avoidance = ysMath::Add(avoidance, ysMath::Mul(ysMath::LoadScalar(10.0f * massRatio), delta));
    }

    return ysMath::Add(ysMath::Add(drag, avoidance), m_externalAcceleration);
}

void dphysics::MSSParticle::RegisterSpring(MSSSpring *spring) {
    m_adjacentSprings.New() = (spring);
}

void dphysics::MSSParticle::UnregisterSpring(MSSSpring *spring) {
    int nSprings = m_adjacentSprings.GetNumObjects();

    for (int i = 0; i < nSprings; i++) {
        if (m_adjacentSprings[i] == spring) {
            m_adjacentSprings.Delete(i);
            return;
        }
    }
}

void dphysics::MSSParticle::DeleteAllConnections() {
    int nSprings = m_adjacentSprings.GetNumObjects();

    for (int i = 0; i < nSprings; i++) {
        MSSParticle *p0 = m_adjacentSprings[i]->GetParticle0();
        MSSParticle *p1 = m_adjacentSprings[i]->GetParticle1();

        if (this == p0) {
            m_adjacentSprings[i]->SetParticle0(NULL);
        }
        else if (this == p1) {
            m_adjacentSprings[i]->SetParticle1(NULL);
        }
        else {
            // Somehow this spring was never connected to this particle...
        }
    }
}

ysVector dphysics::MSSParticle::GetPosition(MassSpringSystem::RK4_PASS pass) const {
    switch (pass) {
    case MassSpringSystem::RK4_FIRST_PASS:
        return m_position;
    case MassSpringSystem::RK4_SECOND_PASS:
    case MassSpringSystem::RK4_THIRD_PASS:
    case MassSpringSystem::RK4_FOURTH_PASS:
        return m_PTemp;
    }

    return m_position;
}

ysVector dphysics::MSSParticle::GetVelocity(MassSpringSystem::RK4_PASS pass) const {
    switch (pass) {
    case MassSpringSystem::RK4_FIRST_PASS:
        return m_velocity;
    case MassSpringSystem::RK4_SECOND_PASS:
    case MassSpringSystem::RK4_THIRD_PASS:
    case MassSpringSystem::RK4_FOURTH_PASS:
        return m_VTemp;
    }

    return m_velocity;
}

ysVector dphysics::MSSParticle::SampleAverageVelocity() {
    m_averageVelocitySamples++;

    m_averageVelocity = ysMath::Add(ysMath::Mul(m_velocity, ysMath::LoadScalar(0.5f)),
        ysMath::Mul(m_averageVelocity, ysMath::LoadScalar(0.5f)));

    return m_averageVelocity;
}

void dphysics::MSSParticle::ClearCollisions() {
    m_collidingParticles.Clear();
}

void dphysics::MSSParticle::AddCollision(MSSParticle *particle) {
    m_collidingParticles.New() = particle;
}

bool dphysics::MSSParticle::IsConnected(MSSParticle *particle) {
    int nConnections = m_adjacentSprings.GetNumObjects();
    int nConenctions2 = particle->m_adjacentSprings.GetNumObjects();

    MSSParticle *particle1 = NULL;
    MSSParticle *particle2 = NULL;

    if (nConnections < nConenctions2) {
        particle1 = this;
        particle2 = particle;
    }
    else {
        particle1 = particle;
        particle2 = this;
    }

    nConnections = particle1->m_adjacentSprings.GetNumObjects();

    for (int i = 0; i < nConnections; i++) {
        if (particle1->m_adjacentSprings[i]->GetParticle0() == particle2 ||
            particle1->m_adjacentSprings[i]->GetParticle1() == particle2) {
            return true;
        }
    }

    return false;
}

// Mass spring system
dphysics::MassSpringSystem::MassSpringSystem() {
    /* void */
}

dphysics::MassSpringSystem::~MassSpringSystem() {
    /* void */
}

void dphysics::MassSpringSystem::SetStep(float step) {
    m_step = step;
    m_halfStep = (0.5f) * m_step;
    m_sixthStep = m_step / 6.0f;
}

dphysics::MSSParticle *dphysics::MassSpringSystem::NewParticle() {
    return m_particles.NewGeneric<MSSParticle, 16>();
}

void dphysics::MassSpringSystem::RemoveParticle(MSSParticle *particle) {
    particle->DeleteAllConnections();
    m_particles.Delete(particle->GetIndex(), true);
}

dphysics::MSSSpring *dphysics::MassSpringSystem::NewSpring() {
    return m_springs.NewGeneric<MSSSpring, 16>();
}

void dphysics::MassSpringSystem::RemoveSpring(MSSSpring *spring) {
    if (spring->GetParticle0() != NULL) {
        spring->GetParticle0()->UnregisterSpring(spring);
    }

    if (spring->GetParticle1() != NULL) {
        spring->GetParticle1()->UnregisterSpring(spring);
    }

    m_springs.Delete(spring->GetIndex());
}

void dphysics::MassSpringSystem::Update() {
    DetectCollisions();

    float startTime = 0.0f;
    float halfTime = m_halfStep;
    float fullTime = m_step;

    ysVector halfStep = ysMath::LoadScalar(m_halfStep);
    ysVector sixthStep = ysMath::LoadScalar(m_sixthStep);

    int i = 0;
    int numParticles = m_particles.GetNumObjects();

    // Compute first step
    for (i = 0; i < numParticles; i++) {
        MSSParticle *particle = m_particles.Get(i);
        if (particle->GetInverseMass() > 0.0f) {
            particle->m_DPTemp1 = particle->GetVelocity();
            particle->m_DVTemp1 = particle->CalculateAcceleration(RK4_FIRST_PASS, startTime);
        }
    }

    for (i = 0; i < numParticles; i++) {
        MSSParticle *particle = m_particles.Get(i);
        if (particle->GetInverseMass() > 0.0f) {
            particle->m_PTemp = ysMath::Add(particle->GetPosition(), ysMath::Mul(halfStep, particle->m_DPTemp1));
            particle->m_VTemp = ysMath::Add(particle->GetVelocity(), ysMath::Mul(halfStep, particle->m_DVTemp1));
        }
        else {
            particle->m_PTemp = particle->GetPosition();
            particle->m_VTemp = ysMath::Constants::Zero;
        }
    }

    // Compute second step
    for (i = 0; i < numParticles; i++) {
        MSSParticle *particle = m_particles.Get(i);
        if (particle->GetInverseMass() > 0.0f) {
            particle->m_DPTemp2 = particle->m_VTemp;
            particle->m_DVTemp2 = particle->CalculateAcceleration(RK4_SECOND_PASS, halfTime);
        }
    }

    for (i = 0; i < numParticles; i++) {
        MSSParticle *particle = m_particles.Get(i);
        if (particle->GetInverseMass() > 0.0f) {
            particle->m_PTemp = ysMath::Add(particle->GetPosition(), ysMath::Mul(halfStep, particle->m_DPTemp2));
            particle->m_VTemp = ysMath::Add(particle->GetVelocity(), ysMath::Mul(halfStep, particle->m_DVTemp2));
        }
        else {
            particle->m_PTemp = particle->GetPosition();
            particle->m_VTemp = ysMath::Constants::Zero;
        }
    }

    // Compute third step
    for (i = 0; i < numParticles; i++) {
        MSSParticle *particle = m_particles.Get(i);
        if (particle->GetInverseMass() > 0.0f) {
            particle->m_DPTemp3 = particle->m_VTemp;
            particle->m_DVTemp3 = particle->CalculateAcceleration(RK4_THIRD_PASS, halfTime);
        }
    }

    for (i = 0; i < numParticles; i++) {
        MSSParticle *particle = m_particles.Get(i);
        if (particle->GetInverseMass() > 0.0f) {
            particle->m_PTemp = ysMath::Add(particle->GetPosition(), ysMath::Mul(halfStep, particle->m_DPTemp3));
            particle->m_VTemp = ysMath::Add(particle->GetVelocity(), ysMath::Mul(halfStep, particle->m_DVTemp3));
        }
        else {
            particle->m_PTemp = particle->GetPosition();
            particle->m_VTemp = ysMath::Constants::Zero;
        }
    }

    // Compute fourth step
    for (i = 0; i < numParticles; i++) {
        MSSParticle *particle = m_particles.Get(i);
        if (particle->GetInverseMass() > 0.0f) {
            particle->m_DPTemp4 = particle->m_VTemp;
            particle->m_DVTemp4 = particle->CalculateAcceleration(RK4_FOURTH_PASS, fullTime);
        }
    }

    for (i = 0; i < numParticles; i++) {
        MSSParticle *particle = m_particles.Get(i);
        if (particle->GetInverseMass() > 0.0f) {
            // Implements:
            // m_position += m_sixthStep * ( m_DPTemp1 + 2 * (m_DPTemp2 + m_DPTemp3 + m_DPTemp4) )

            ysVector pCalc = ysMath::Add(ysMath::Add(particle->m_DPTemp2, particle->m_DPTemp3), particle->m_DPTemp4);
            pCalc = ysMath::Mul(pCalc, ysMath::Constants::Double);
            pCalc = ysMath::Add(pCalc, particle->m_DPTemp1);
            pCalc = ysMath::Mul(pCalc, sixthStep);

            particle->m_position = ysMath::ExtendVector(ysMath::Add(pCalc, particle->m_position));

            ysVector vCalc = ysMath::Add(ysMath::Add(particle->m_DVTemp2, particle->m_DVTemp3), particle->m_DVTemp4);
            vCalc = ysMath::Mul(vCalc, ysMath::Constants::Double);
            vCalc = ysMath::Add(vCalc, particle->m_DVTemp1);
            vCalc = ysMath::Mul(vCalc, sixthStep);

            particle->m_velocity = ysMath::ExtendVector(ysMath::Add(vCalc, particle->m_velocity));
        }
    }

    int numSprings = m_springs.GetNumObjects();
    for (int i = 0; i < numSprings; i++) {
        m_springs.Get(i)->Update(m_step);
    }
}

void dphysics::MassSpringSystem::DetectCollisions() {
    int numParticles = m_particles.GetNumObjects();
    int i = 0;

    for (i = 0; i < numParticles; i++) {
        m_particles.Get(i)->ClearCollisions();
    }

    for (i = 0; i < numParticles; i++) {
        MSSParticle *particle1 = m_particles.Get(i);

        for (int j = i + 1; j < numParticles; j++) {
            MSSParticle *particle2 = m_particles.Get(j);

            if (particle1->GetCollisionEnable() && particle2->GetCollisionEnable()) {
                float radius1 = particle1->GetRadius();
                float radius2 = particle2->GetRadius();

                if (particle1->IsConnected(particle2)) {
                    continue;
                }

                float r = (radius1 + radius2);
                r *= r;

                ysVector delta = ysMath::Sub(particle1->GetPosition(), particle2->GetPosition());
                float distance2 = ysMath::GetScalar(ysMath::MagnitudeSquared3(delta));

                if (distance2 < r) {
                    particle1->AddCollision(particle2);
                    particle2->AddCollision(particle1);
                }
            }
        }
    }
}
