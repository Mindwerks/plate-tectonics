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

#ifndef WORLD_POINT_HPP
#define WORLD_POINT_HPP

#include "utils.hpp"

class WorldDimension;

/// Immutable point expressed in World coordinates
class WorldPoint {
  public:
    WorldPoint(uint32_t x, uint32_t y, const WorldDimension& dim);
    WorldPoint(const WorldPoint& other);
    uint32_t x() const;
    uint32_t y() const;
    uint32_t toIndex(const WorldDimension&) const;

  private:
    const uint32_t _x;
    const uint32_t _y;
};

#endif
