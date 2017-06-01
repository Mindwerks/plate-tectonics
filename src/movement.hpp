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

#define NOMINMAX

#include <vector>
#include <cmath>     // sin, cos
#include "simplerandom.hpp"
#include "utils.hpp"
#include "bounds.hpp"
#include "dimension.h"
#include "mass.hpp"

class IPlate;


class IMovement
{
public:
    virtual Platec::vec2f velocityUnitVector() const = 0;
    virtual void decImpulse(const Platec::vec2f& delta) = 0;
};

class Movement : public IMovement {
    
private:
    SimpleRandom randSource;
    float_t velocity;       ///< Plate's velocity.
    const signed char rotDir;        ///< Direction of rotation: 1 = CCW, -1 = ClockWise.
    Platec::vec2f accVec = Platec::vec2f(0.f, 0.f);    ///< X and Y components of plate's acceleration vector.
    Platec::vec2f velVec = Platec::vec2f(0.f, 0.f); ///< X and Y components of plate's direction unit vector.
    float_t DeformationWeight = 2.f;
    
public:
    Movement(SimpleRandom randsource);
    void applyFriction(const float_t deformed_mass,const float_t mass);
    void move(const Dimension& worldDimension);
    Platec::vec2f velocityUnitVector() const;
    Platec::vec2f velocityVector() const;

    Platec::vec2f velocityOn(const float_t length) const;
    float_t dot(const Platec::vec2f& dotVector) const;
    float_t momentum(const Mass& mass) const;
    float_t getVelocity() const;;

    void collide(const IMass& thisMass, IPlate& p,const float_t coll_mass);

    void addImpulse(const Platec::vec2f& impulse);
    void decImpulse(const Platec::vec2f& delta);
    void setDeformationWeight(float deformationWeight);
    float getDeformationWeight() const;

};


#endif
