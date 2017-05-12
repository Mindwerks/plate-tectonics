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

#define NOMINMAX


#include <stdio.h>
#include <utility>

#include "simplerandom.hpp"
#include "heightmap.hpp"
#include "rectangle.hpp"
#include "segment_data.hpp"
#include "utils.hpp"
#include "geometry.hpp"



/// Plate bounds.
class Bounds
{
    
private:
    
    const WorldDimension _worldDimension;
    Platec::Point2D<float_t> _position;
    Dimension _dimension;
    
public:

    /// @param worldDimension dimension of the world containing the plate
    /// @param position Position of the top left corner of the plae
    /// @param dimension Dimension of the plate
    Bounds(const WorldDimension& worldDimension,
           const Platec::Point2D<float_t>& position,
           const Dimension& dimension);

    /// Accept plate relative coordinates and return the index inside the plate.
    /// The index can be used with other classes to retrieve information about specific points.
    /// Throw an exception if the coordinates are not valid.
    uint32_t index(const Platec::Point2D<uint32_t>& p) const;
    
    /// Total area occupied by the plate (width * height).
    uint32_t area() const;
    
    /// Width of the plate.
    uint32_t width() const;

    /// Height of the plate.    
    uint32_t height() const;
    
    /// Left position of the Plate in world coordinates.
    uint32_t left() const;
    
    /// Top position of the Plate in world coordinates.
    uint32_t top() const;
    
    /// Right position of the Plate in world coordinates.
    uint32_t right() const;
    
    /// Bottom position of the Plate in world coordinates.
    uint32_t bottom() const;
    
    /// First point NOT part of the plate (on the right).
    /// It is expressed in world coordinates.    
    uint32_t rightAsUintNonInclusive() const;
    
    /// First point NOT part of the plate (on the bottom).
    /// It is expressed in world coordinates.    
    uint32_t bottomAsUintNonInclusive() const;
    
    /// Given a point in World relative coordinates, it tells if it is part of the plate or not.   
    bool containsWorldPoint(const Platec::Point2D<uint32_t>& p) const;
    
    /// Given a point in plate relative coordinates, it tells if it is part of the plate or not.    
    bool isInLimits(const Platec::Point2D<uint32_t>& p) const;
    
    /// Shift the position of the top left corner by the given amount.
    /// It preserves the dimension of the plate.    
    void shift(const Platec::Vector2D<float_t>& delta);
    
     /// Grow the plate towards the right and the bottom.   
    void grow(const Platec::Vector2D<uint32_t>& delta);
    
    /// Translate world coordinates into offset within plate's height map.
    ///
    /// If the global world map coordinates are within plate's height map,
    /// the values of passed coordinates will be altered to contain the
    /// X and y offset within the plate's height map. Otherwise an exception is thrown.
    ///
    /// @param[in, out] x   Offset on the global world map along X axis.
    /// @param[in, out] y   Offset on the global world map along Y axis.
    /// @return             Offset in height map or BAD_INDEX on error.    
    std::pair<uint32_t, Platec::Point2D<uint32_t>>
            getValidMapIndex(const Platec::Point2D<uint32_t>& p) const;
    
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
    std::pair<uint32_t, Platec::Point2D<uint32_t>> getMapIndex
            (const Platec::Point2D<uint32_t>& p) const;

};

#endif
