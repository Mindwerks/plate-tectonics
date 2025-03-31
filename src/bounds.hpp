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

#ifndef BOUNDS_HPP
#define BOUNDS_HPP

#include <stdio.h>

#include "simplerandom.hpp"
#include "heightmap.hpp"
#include "rectangle.hpp"
#include "segment_data.hpp"
#include "utils.hpp"

/// Represent the bounds of a Plate.
class IBounds {
public:
    virtual ~IBounds() {}

    /// Accept plate relative coordinates and return the index inside the plate.
    /// The index can be used with other classes to retrieve information about specific points.
    /// Throw an exception if the coordinates are not valid.
    virtual uint32_t index(uint32_t px, uint32_t py) const = 0;

    /// Total area occupied by the plate (width * height).
    virtual uint32_t area() const = 0;

    /// Width of the plate.
    virtual uint32_t width() const = 0;

    /// Height of the plate.
    virtual uint32_t height() const = 0;

    /// Left position of the Plate in world coordinates.
    virtual uint32_t leftAsUint() const = 0;

    /// Top position of the Plate in world coordinates.
    virtual uint32_t topAsUint() const = 0;

    /// First point NOT part of the plate (on the right).
    /// It is expressed in world coordinates.
    virtual uint32_t rightAsUintNonInclusive() const = 0;

    /// First point NOT part of the plate (on the bottom).
    /// It is expressed in world coordinates.
    virtual uint32_t bottomAsUintNonInclusive() const = 0;

    /// Given a point in World relative coordinates, it tells if it is part of the plate or not.
    virtual bool containsWorldPoint(uint32_t x, uint32_t y) const = 0;

    /// Given a point in plate relative coordinates, it tells if it is part of the plate or not.
    virtual bool isInLimits(float x, float y) const = 0;

    /// Shift the position of the top left corner by the given amount.
    /// It preserves the dimension of the plate.
    virtual void shift(float dx, float dy) = 0;

    /// Grow the plate towards the right and the bottom.
    /// @param dx must be positive or zero
    /// @param dy must be positive or zero
    virtual void grow(int dx, int dy) = 0;

    /// Translate world coordinates into offset within plate's height map.
    ///
    /// If the global world map coordinates are within plate's height map,
    /// the values of passed coordinates will be altered to contain the
    /// X and y offset within the plate's height map. Otherwise an exception is thrown.
    ///
    /// @param[in, out] x   Offset on the global world map along X axis.
    /// @param[in, out] y   Offset on the global world map along Y axis.
    /// @return             Offset in height map or BAD_INDEX on error.
    virtual uint32_t getValidMapIndex(uint32_t* px, uint32_t* py) const = 0;

    /// Translate world coordinates into offset within plate's height map.
    ///
    /// Iff the global world map coordinates are within plate's height map,
    /// the values of passed coordinates will be altered to contain the
    /// X and y offset within the plate's height map. Otherwise values are
    /// left intact.
    ///
    /// @param[in, out] x   Offset on the global world map along X axis.
    /// @param[in, out] y   Offset on the global world map along Y axis.
    /// @return             Offset in height map
    virtual uint32_t getMapIndex(uint32_t* x, uint32_t* y) const = 0;
};

/// Plate bounds.
class Bounds : public IBounds
{
public:

    /// @param worldDimension dimension of the world containing the plate
    /// @param position Position of the top left corner of the plae
    /// @param dimension Dimension of the plate
    Bounds(const WorldDimension& worldDimension,
           const FloatPoint& position,
           const Dimension& dimension);

    uint32_t index(uint32_t x, uint32_t y) const;
    uint32_t area() const;
    uint32_t width() const;
    uint32_t height() const;
    uint32_t leftAsUint() const;
    uint32_t topAsUint() const;
    uint32_t rightAsUintNonInclusive() const;
    uint32_t bottomAsUintNonInclusive() const;
    bool containsWorldPoint(uint32_t x, uint32_t y) const;
    bool isInLimits(float x, float y) const;
    void shift(float dx, float dy);
    void grow(int dx, int dy);
    uint32_t getValidMapIndex(uint32_t* px, uint32_t* py) const;
    uint32_t getMapIndex(uint32_t* x, uint32_t* y) const;

private:

    /// Return a rectangle representing the Bounds inside the world.
    Platec::Rectangle asRect() const;

    const WorldDimension _worldDimension;
    FloatPoint _position;
    Dimension _dimension;
};

#endif
