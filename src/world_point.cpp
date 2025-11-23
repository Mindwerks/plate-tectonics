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

#include "world_point.hpp"
#include "rectangle.hpp"

WorldPoint::WorldPoint(uint32_t x, uint32_t y, const WorldDimension& dim) : _x(x), _y(y) {
    ASSERT(_x < dim.getWidth() && _y < dim.getHeight(), "Point outside of world!");
}

WorldPoint::WorldPoint(const WorldPoint& other) : _x(other.x()), _y(other.y()) {}

uint32_t WorldPoint::x() const {
    return _x;
}

uint32_t WorldPoint::y() const {
    return _y;
}

uint32_t WorldPoint::toIndex(const WorldDimension& dim) const {
    ASSERT(_x < dim.getWidth() && _y < dim.getHeight(), "Point outside of world!");
    return _y * dim.getWidth() + _x;
}
