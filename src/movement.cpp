/******************************************************************************
*  plate-tectonics, a plate tectonics simulation library
*  Copyright (C) 2012-2013 Lauri Viitanen
*  Copyright (C) 2014-2015 Federico Tomassetti, Bret Curtis
*
*  This library is free software; you can redistribute it and/or
*  modify it under the terms of the GNU Lesser General Public
*  License as published by the Free Software Foundation; either
*  version 2.1 of the License, or (at your option) any later version.
*
*  This library is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*  Lesser General Public License for more details.
*
*  You should have received a copy of the GNU Lesser General Public
*  License along with this library; if not, see http://www.gnu.org/licenses/
*****************************************************************************/

#include <complex>

#include "movement.hpp"
#include "plate.hpp"
#include "mass.hpp"

// Missing on Windows
#ifndef M_PI
#define M_PI 3.141592654
#endif

Movement::Movement(SimpleRandom randsource)
    : randSource(randsource),
      velocity(1.0),
      rotDir(randsource.next() % 2 ? 1 : -1)
{
    const double angle = 2.0 * M_PI * randSource.next_double();
    velVec = Platec::vec2f(cosf(angle),sinf(angle));
}

void Movement::applyFriction(float_t deformed_mass, float_t mass) {
    if (0.0f == mass) {
        velocity = 0.f;
        return;
    }
    
    const auto vel_dec = DeformationWeight * deformed_mass / mass;
    
    if(vel_dec < velocity) {
        velocity -= vel_dec;
    } else {
        velocity = 0.f;
    }
//    vel_dec = vel_dec < velocity ? vel_dec : velocity;
//
//    // Altering the source variable causes the order of calls to
//    // this function to have difference when it shouldn't!
//    // However, it's a hack well worth the outcome. :)
//    velocity -= vel_dec;
}

void Movement::move(const Dimension& worldDimension) {
    // Apply any new impulses to the plate's trajectory.

    velVec = velVec + accVec;
    accVec = Platec::vec2f(0.0, 0.0);


    // Force direction of plate to be unit vector.
    // Update velocity so that the distance of movement doesn't change.
    auto len = velVec.length();
    ASSERT(len != 0.f, "Velocity is zero!");
    // MK: Calculating the inverse length and multiplying changes the output data for maps!
    // I have held off on optimizations like these until the more important optimizations
    // are finished
    velVec = velVec / len;
    
    velocity += len - 1.f;
    velocity = std::max(0.f,velocity); // Round negative values to zero.

    // Apply some circular motion to the plate.
    // Force the radius of the circle to remain fixed by adjusting
    // angular velocity (which depends on plate's velocity).
    const float_t world_avg_side = (worldDimension.getWidth() + worldDimension.getHeight()) / 2.f;
    const float_t alpha = velocity / (world_avg_side * 0.33f);
    const float_t alpha_vel = rotDir * alpha * velocity;
    const float_t cosVel = cosf(alpha_vel);
    const float_t sinVel = sinf(alpha_vel);

    velVec = Platec::vec2f(velVec.x() * cosVel - velVec.y() * sinVel,
                            velVec.y() * cosVel + velVec.x() * sinVel);
    
}

void Movement::addImpulse(const Platec::vec2f& impulse) {
    accVec = accVec + impulse;
}

void Movement::decImpulse(const Platec::vec2f& delta) {
    accVec = accVec - delta;
}

void Movement::setDeformationWeight(float deformationWeight) {
    this->DeformationWeight = deformationWeight;
}

float Movement::getDeformationWeight() const {
    return DeformationWeight;
}

float_t Movement::dot(const Platec::vec2f& dotVector) const {
    return velVec.dotProduct(dotVector);
}

float_t Movement::getVelocity() const {
    return velocity;
}

Platec::vec2f Movement::velocityOn(const float_t length)  const {
    return velVec * length;
}

Platec::vec2f Movement::velocityUnitVector() const {
    return velVec;
}

Platec::vec2f Movement::velocityVector() const {
    return velVec * velocity;
}

float_t Movement::momentum(const Mass& mass) const {
    return mass.getMass() * velocity;
}

void Movement::collide(const IMass& thisMass,
                       IPlate& otherPlate, float_t coll_mass) {
    const auto coeff_rest = 0.f; // Coefficient of restitution.
    // 1 = fully elastic, 0 = stick together.
    const auto massCentersDistance = otherPlate.massCenter() - thisMass.massCenter();
    const auto distance = massCentersDistance.length();
    if (distance <= 0) {
        return; // Avoid division by zero!
    }

    // Scaling is required at last when impulses are added to plates!
    // Compute relative velocity between plates at the collision point.
    // Because torque is not included, calc simplifies to v_ab = v_a - v_b.
    const auto collisionDirection = massCentersDistance/distance;
    const auto relativeVelocity = velocityUnitVector() - otherPlate.velocityUnitVector();

    // Get the dot product of relative velocity vector and collision vector.
    // Then get the projection of v_ab along collision vector.
    // Note that vector n must be a unit vector!
    const auto rel_dot_n = collisionDirection.dotProduct(relativeVelocity);
    if (rel_dot_n <= 0) {
        return; // Exit if objects are moving away from each other.
    }

    // Calculate the denominator of impulse: n . n * (1 / m_1 + 1 / m_2).
    // Use the mass of the colliding crust for the "donator" plate.
    // MK: Is this a bug? collisionDirection has length 1 because it's a unit vector
    // I have kept the old code here just in case a float_t roundoff would change the map
    const float_t col_len = std::pow(collisionDirection.length(), 2);
    const float_t denom = col_len * (1.f / otherPlate.getMass() + 1.f / coll_mass);

    // Calculate force of impulse.
    const float_t J = -(1 + coeff_rest) * rel_dot_n / denom;

    // Compute final change of trajectory.
    // The plate that is the "giver" of the impulse should receive a
    // force according to its pre-collision mass, not the current mass!
    addImpulse(collisionDirection * (J / thisMass.getMass()));
    otherPlate.decImpulse(collisionDirection * (J / (coll_mass + otherPlate.getMass())));

    // In order to prove that the code above works correctly, here is an
    // example calculation with ball A (mass 10) moving right at velocity
    // 1 and ball B (mass 100) moving up at velocity 1. Collision point
    // is at rightmost point of ball A and leftmost point of ball B.
    // Radius of both balls is 2.
    // ap_dx =  2;
    // ap_dy =  0;
    // bp_dx = -2;
    // bp_dy =  0;
    // nx = 2 - -2 = 4;
    // ny = 0 -  0 = 0;
    // n_len = sqrt(4 * 4 + 0) = 4;
    // nx = 4 / 4 = 1;
    // ny = 0 / 4 = 0;
    //
    // So far so good, right? Normal points into ball B like it should.
    //
    // rel_vx = 1 -  0 = 1;
    // rel_vy = 0 - -1 = 1;
    // rel_dot_n = 1 * 1 + 1 * 0 = 1;
    // denom = (1 * 1 + 0 * 0) * (1/10 + 1/100) = 1 * 11/100 = 11/100;
    // J = -(1 + 0) * 1 / (11/100) = -100/11;
    // dx = 1 * (-100/11) / 10 = -10/11;
    // dy = 0;
    // p.dx = -1 * (-100/11) / 100 = 1/11;
    // p.dy = -0;
    //
    // So finally:
    // vx = 1 - 10/11 = 1/11
    // vy = 0
    // p.vx = 0 + 1/11 = 1/11
    // p.vy = -1
    //
    // We see that in with restitution 0, both balls continue at same
    // speed along X axis. However at the same time ball B continues its
    // path upwards like it should. Seems correct right?
}
