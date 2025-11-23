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

#include "rectangle.hpp"
#include "utils.hpp"
#include <stdexcept>

namespace Platec {

/// Return a valid index or BAD_INDEX
uint32_t Rectangle::getMapIndex(uint32_t* px, uint32_t* py) const {
    uint32_t world_width = _worldDimension.getWidth();
    uint32_t world_height = _worldDimension.getHeight();
    uint32_t x = *px % world_width;
    uint32_t y = *py % world_height;

    const uint32_t ilft = (uint32_t)(int)_left;
    const uint32_t itop = (uint32_t)(int)_top;
    const uint32_t irgt =
        (uint32_t)(int)_right + (((uint32_t)(int)_right < ilft) ? (uint32_t)(int)world_width : 0);
    const uint32_t ibtm = (uint32_t)(int)_bottom +
                          (((uint32_t)(int)_bottom < itop) ? (uint32_t)(int)world_height : 0);
    const int width = irgt - ilft;
    ASSERT(width >= 0, "Width must be postive");

    ///////////////////////////////////////////////////////////////////////
    // If you think you're smart enough to optimize this then PREPARE to be
    // smart as HELL to debug it!
    ///////////////////////////////////////////////////////////////////////

    const uint32_t x_plus_width = x + world_width;
    const uint32_t xOkA = (x >= ilft) && (x < irgt);
    const uint32_t xOkB = (x_plus_width >= ilft) && (x_plus_width < irgt);
    const uint32_t xOk = xOkA || xOkB;

    const uint32_t y_plus_height = y + world_height;
    const uint32_t yOkA = (y >= itop) && (y < ibtm);
    const uint32_t yOkB = (y_plus_height >= itop) && (y_plus_height < ibtm);
    const uint32_t yOk = yOkA || yOkB;

    x += (x < ilft) ? world_width : 0;  // Point is within plate's map: wrap
    y += (y < itop) ? world_height : 0; // it around world edges if necessary.

    ASSERT(x >= ilft && y >= itop, "Coordinates must be positive");
    x -= ilft; // Calculate offset within local map.
    y -= itop;

    if (xOk && yOk) {
        *px = x;
        *py = y;
        return (y * width + x);
    } else {
        return BAD_INDEX;
    }
}

void Rectangle::enlarge_to_contain(uint32_t x, uint32_t y) {
    if (y < _top) {
        _top = y;
    } else if (y > _bottom) {
        _bottom = y;
    }
    if (x < _left) {
        _left = x;
    } else if (x > _right) {
        _right = x;
    }
}

} // namespace Platec