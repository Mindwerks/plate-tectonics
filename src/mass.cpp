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

#include "mass.hpp"

#include <algorithm>
#include "vector2D.h"
// ----------------------------------------------
// MassBuilder
// ----------------------------------------------

MassBuilder::MassBuilder(const HeightMap& map)
    : mass(0), center(0.f, 0.f) {
    uint32_t index = 0;
    for (const auto& data : map.getData()) {
        addPoint(map.getDimension().coordOF(index),data);
        ++index;
    }
}

MassBuilder::MassBuilder()
    : mass(0), center(0.f, 0.f) {
}

void MassBuilder::addPoint(const Platec::vec2ui& point,
                            const float crust) {
    auto testCrust = std::max(crust, 0.f);
    mass += testCrust;
    // Update the center coordinates weighted by mass.
    //Explicit convertsion from int to float
    center.shift(Platec::vec2f(static_cast<float_t>(point.x()), 
                               static_cast<float_t>(point.y())) * testCrust);
}

Mass MassBuilder::build() {
    if (mass <= 0.f) {
        return Mass(0.f, Platec::vec2f(0.0, 0.0));
    }
    float inv_mass = 1 / mass;

    return Mass(mass, center * inv_mass);
}

// ----------------------------------------------
// Mass
// ----------------------------------------------

Mass::Mass(float mass, Platec::vec2f center) :
        mass(mass), center(center) {
}

const Platec::vec2f Mass::massCenter() const {
    return center;
}


void Mass::incMass(float delta) {
    mass = std::max(0.0f, mass + delta);
}

float Mass::getMass() const {
    return mass;
}

bool Mass::isNull() const {
    return mass <= 0.f;
}
