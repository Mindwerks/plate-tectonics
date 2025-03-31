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

#include "movement.hpp"
#include "plate.hpp"
#include "mass.hpp"

// Missing on Windows
#ifndef M_PI
#define M_PI 3.141592654
#endif

#if (defined(_MSC_VER) && defined(_M_X64)) || \
    (defined(__APPLE__) && defined(__clang__))
#define sin(x) static_cast<float>(sin(static_cast<double>(x)))
#define cos(x) static_cast<float>(cos(static_cast<double>(x)))
#endif

Movement::Movement(SimpleRandom randsource, const WorldDimension& worldDimension)
    : _randsource(randsource),
      velocity(1),
      rot_dir(randsource.next() % 2 ? 1 : -1),
      dx(0), dy(0),
      _worldDimension(worldDimension) {
    const double angle = 2 * M_PI * _randsource.next_double();
    vx = cos(angle) * INITIAL_SPEED_X;
    vy = sin(angle) * INITIAL_SPEED_X;
}

void Movement::applyFriction(float deformed_mass, float mass) {
    if (0.0f == mass) {
        velocity = 0;
        return;
    }
    float vel_dec = DEFORMATION_WEIGHT * deformed_mass / mass;
    vel_dec = vel_dec < velocity ? vel_dec : velocity;

    // Altering the source variable causes the order of calls to
    // this function to have difference when it shouldn't!
    // However, it's a hack well worth the outcome. :)
    velocity -= vel_dec;
}

void Movement::move() {
    // Apply any new impulses to the plate's trajectory.
    vx += dx;
    vy += dy;
    dx = 0;
    dy = 0;

    // Force direction of plate to be unit vector.
    // Update velocity so that the distance of movement doesn't change.
    float len = sqrt(vx*vx + vy*vy);
    ASSERT(len > 0, "Velocity is zero!");
    // MK: Calculating the inverse length and multiplying changes the output data for maps!
    // I have held off on optimizations like these until the more important optimizations
    // are finished
    vx /= len;
    vy /= len;
    velocity += len - 1.0;
    velocity *= velocity > 0; // Round negative values to zero.

    // Apply some circular motion to the plate.
    // Force the radius of the circle to remain fixed by adjusting
    // angular velocity (which depends on plate's velocity).
    uint32_t world_avg_side = (_worldDimension.getWidth() + _worldDimension.getHeight()) / 2;
    float alpha = rot_dir * velocity / (world_avg_side * 0.33);
    float alpha_vel = alpha * velocity;
    float _cos = cos(alpha_vel);
    float _sin = sin(alpha_vel);
    float _vx = vx * _cos - vy * _sin;
    float _vy = vy * _cos + vx * _sin;
    vx = _vx;
    vy = _vy;
}

float Movement::velocityOnX() const {
    return vx * velocity;
}

float Movement::velocityOnY() const {
    return vy * velocity;
}

float Movement::velocityOnX(float length) const {
    ASSERT(length >= 0, "Negative length makes no sense");
    return vx * length;
}

float Movement::velocityOnY(float length) const {
    ASSERT(length >= 0, "Negative length makes no sense");
    return vy * length;
}

float Movement::dot(float dx_, float dy_) const {
    return vx * dx_ + vy * dy_;
}

float Movement::relativeUnitVelocityOnX(float otherVx) const {
    return vx - otherVx;
}

float Movement::relativeUnitVelocityOnY(float otherVy) const {
    return vy - otherVy;
}

float Movement::momentum(const Mass& mass) const throw() {
    return mass.getMass() * velocity;
}

void Movement::collide(const IMass& thisMass,
                       IPlate& otherPlate,
                       uint32_t wx, uint32_t wy, float coll_mass) {
    const float coeff_rest = 0.0; // Coefficient of restitution.
    // 1 = fully elastic, 0 = stick together.
    Platec::IntVector massCentersDistance =
        otherPlate.massCenter().toInt() - thisMass.massCenter().toInt();
    float distance = massCentersDistance.length();
    if (distance <= 0) {
        return; // Avoid division by zero!
    }

    // Scaling is required at last when impulses are added to plates!
    // Compute relative velocity between plates at the collision point.
    // Because torque is not included, calc simplifies to v_ab = v_a - v_b.
    Platec::FloatVector collisionDirection = Platec::FloatVector(massCentersDistance.x()/distance,massCentersDistance.y()/distance);
    Platec::FloatVector relativeVelocity = velocityUnitVector() - otherPlate.velocityUnitVector();

    // Get the dot product of relative velocity vector and collision vector.
    // Then get the projection of v_ab along collision vector.
    // Note that vector n must be a unit vector!
    const float rel_dot_n = collisionDirection.dotProduct(relativeVelocity);
    if (rel_dot_n <= 0) {
        return; // Exit if objects are moving away from each other.
    }

    // Calculate the denominator of impulse: n . n * (1 / m_1 + 1 / m_2).
    // Use the mass of the colliding crust for the "donator" plate.
    // MK: Is this a bug? collisionDirection has length 1 because it's a unit vector
    // I have kept the old code here just in case a float roundoff would change the map
    float col_len = collisionDirection.length();
    float denom = col_len * col_len * (1.0 / otherPlate.getMass() + 1.0 / coll_mass);

    // Calculate force of impulse.
    float J = -(1 + coeff_rest) * rel_dot_n / denom;

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
