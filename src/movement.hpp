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

#ifndef MOVEMENT_HPP
#define MOVEMENT_HPP

#include <vector>
#include <cmath>     // sin, cos
#include "simplerandom.hpp"
#include "heightmap.hpp"
#include "rectangle.hpp"
#include "segment_data.hpp"
#include "utils.hpp"
#include "bounds.hpp"
#include "dimension.h"

#define CONT_BASE 1.0 ///< Height limit that separates seas from dry land.
#define INITIAL_SPEED_X 1
#define DEFORMATION_WEIGHT 2

typedef uint32_t ContinentId;

class IPlate;
class plate;
class IMass;
class Mass;

class IMovement
{
public:
    virtual Platec::Vector2D<float_t> velocityUnitVector() const = 0;
    virtual void decImpulse(const Platec::Vector2D<float_t>& delta) = 0;
};

class Movement : public IMovement
{
public:
    Movement(SimpleRandom randsource, const Dimension& worldDimension);
    void applyFriction(float deformed_mass, float mass);
    void move();
    Platec::Vector2D<float_t> velocityUnitVector() const {
        return Platec::Vector2D<float_t>(vx, vy);
    }
    Platec::Vector2D<float_t> velocityVector() const {
        return Platec::Vector2D<float_t>(vx * velocity, vy * velocity);
    }
    float velocityOnX() const;
    float velocityOnY() const;
    float velocityOnX(float length) const;
    float velocityOnY(float length) const;
    float dot(float dx_, float dy_) const;
    float momentum(const Mass& mass) const throw();
    float getVelocity() const {
        return velocity;
    };
    /// @Deprecated, use velocityUnitVector instead
    float velX() const throw() {
        return vx;
    }
    /// @Deprecated, use velocityUnitVector instead
    float velY() const throw() {
        return vy;
    }
    void collide(const IMass& thisMass, IPlate& p, uint32_t wx, uint32_t wy, float coll_mass);
    void decDx(float delta) {
        dx -= delta;
    }
    void decDy(float delta) {
        dy -= delta;
    }
    void addImpulse(const Platec::Vector2D<float_t>& impulse) {
        dx += impulse.x();
        dy += impulse.y();
    }
    void decImpulse(const Platec::Vector2D<float_t>& delta) {
        dx -= delta.x();
        dy -= delta.y();
    };
private:
    float relativeUnitVelocityOnX(float otherVx) const;
    float relativeUnitVelocityOnY(float otherVy) const;

    SimpleRandom _randsource;
    const Dimension _worldDimension;
    float velocity;       ///< Plate's velocity.
    float rot_dir;        ///< Direction of rotation: 1 = CCW, -1 = ClockWise.
    float dx, dy;         ///< X and Y components of plate's acceleration vector.
    float vx, vy;         ///< X and Y components of plate's direction unit vector.
};


#endif
