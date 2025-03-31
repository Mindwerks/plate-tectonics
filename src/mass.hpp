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

#ifndef MASS_HPP
#define MASS_HPP

#include <vector>
#include <cmath>     // sin, cos
#include "simplerandom.hpp"
#include "heightmap.hpp"
#include "rectangle.hpp"
#include "segment_data.hpp"
#include "utils.hpp"
#include "bounds.hpp"
#include "movement.hpp"

class Mass;

class MassBuilder
{
public:
    // FIXME: take a HeightMap instead of float*
    MassBuilder(const float* m, const Dimension& dimension);
    MassBuilder();
    void addPoint(uint32_t x, uint32_t y, float crust);
    Mass build();
private:
    float mass;           ///< Amount of crust that constitutes the plate.
    float cx, cy;         ///< X and Y components of the center of mass of plate.
};

class IMass
{
public:
    virtual ~IMass() {}
    virtual float getMass() const = 0;
    virtual FloatPoint massCenter() const = 0;
};

class Mass : public IMass
{
public:
    Mass(float mass_, float cx_, float cy_);
    void incMass(float delta);
    float getMass() const;
    float getCx() const;
    float getCy() const;
    FloatPoint massCenter() const {
        return FloatPoint(cx, cy);
    }
    bool null() const;
private:
    float mass;           ///< Amount of crust that constitutes the plate.
    float cx, cy;         ///< X and Y components of the center of mass of plate.
    float _totalX, _totalY;
};

#endif
