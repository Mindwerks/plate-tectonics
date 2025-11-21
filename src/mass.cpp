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

// ----------------------------------------------
// MassBuilder
// ----------------------------------------------

MassBuilder::MassBuilder(const float* m, const Dimension& dimension)
    : mass(0), cx(0), cy(0)
{
    uint32_t k;
    for (uint32_t y = k = 0; y < dimension.getHeight(); ++y) {
        for (uint32_t x = 0; x < dimension.getWidth(); ++x, ++k) {
            ASSERT(m[k] >= 0.0f, "Crust should be not negative");
            addPoint(x, y, m[k]);
        }
    }
}

MassBuilder::MassBuilder()
    : mass(0), cx(0), cy(0)
{

}

void MassBuilder::addPoint(uint32_t x, uint32_t y, float crust)
{
    ASSERT(crust >= 0.0f, "Crust should be not negative");
    mass += crust;
    // Update the center coordinates weighted by mass.
    cx += x * crust;
    cy += y * crust;
}

Mass MassBuilder::build()
{
    if (mass <= 0) {
        return Mass(0, 0, 0);
    } else {
        ASSERT(mass > 0, "Mass was zero!");
        return Mass(mass, cx / mass, cy / mass);
    }
}

// ----------------------------------------------
// Mass
// ----------------------------------------------

Mass::Mass(float mass_, float cx_, float cy_)
    : mass(mass_), cx(cx_), cy(cy_)
{

}

void Mass::incMass(float delta)
{
    mass += delta;
    // Clamp negative mass to zero to handle floating point precision errors
    // that accumulate over many iterations (Issue #30)
    // For large maps (512x512+) with many plates (10+) and long simulations,
    // errors can accumulate significantly. Since mass is recalculated from
    // heightmaps during erosion cycles, accepting small negative values and
    // clamping them to zero is acceptable.
    if (mass < 0.0f) {
        mass = 0.0f;
    }
}

float Mass::getMass() const
{
    return mass;
}

float Mass::getCx() const
{
    return cx;
}

float Mass::getCy() const
{
    return cy;
}

bool Mass::null() const
{
    return mass <= 0;
}

