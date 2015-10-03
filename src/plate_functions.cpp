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

#include "plate_functions.hpp"

void calculateCrust(
    uint32_t x, uint32_t y,
    uint32_t index,
    float& w_crust, float& e_crust, float& n_crust, float& s_crust,
    uint32_t& w, uint32_t& e, uint32_t& n, uint32_t& s,
    const WorldDimension& worldDimension,
    HeightMap& map,
    const uint32_t width,  const uint32_t height)
{
    try {
        // Build masks for accessible directions (4-way).
        // Allow wrapping around map edges if plate has world wide dimensions.
        const uint32_t world_width = worldDimension.getWidth();
        const uint32_t world_height = worldDimension.getHeight();
        bool width_bit = width == world_width;
        bool height_bit = height == world_height;
        uint32_t w_mask = -((x > 0)          | width_bit);
        uint32_t e_mask = -((x < width - 1)  | width_bit);
        uint32_t n_mask = -((y > 0)          | height_bit);
        uint32_t s_mask = -((y < height - 1) | height_bit);

        // Calculate the x and y offset of neighbour directions.
        // If neighbour is out of plate edges, set it to zero. This protects
        // map memory reads from segment faulting.
        const uint32_t x_mod = x % world_width;
        const uint32_t y_mod = y % world_height;
        uint32_t x_mod_minus_1 = x_mod == 0 ? world_width - 1 : x_mod - 1;
        uint32_t x_mod_plus_1 = x_mod + 1 == world_width ? 0 : x_mod + 1;
        uint32_t y_mod_minus_1 = y_mod == 0 ? world_height - 1 : y_mod - 1;
        uint32_t y_mod_plus_1 = y_mod + 1 == world_height ? 0 : y_mod + 1;
        w = w_mask==-1 ? x_mod_minus_1 : 0;
        e = e_mask==-1 ? x_mod_plus_1 : 0;
        n = n_mask==-1 ? y_mod_minus_1 : 0;
        s = s_mask==-1 ? y_mod_plus_1 : 0;

        // Calculate offsets within map memory.
        w = y * width + w;
        e = y * width + e;
        n = n * width + x;
        s = s * width + x;

        // Extract neighbours heights. Apply validity filtering: 0 is invalid.
        w_crust = map[w] * (w_mask & (map[w] < map[index]));
        e_crust = map[e] * (e_mask & (map[e] < map[index]));
        n_crust = map[n] * (n_mask & (map[n] < map[index]));
        s_crust = map[s] * (s_mask & (map[s] < map[index]));
    } catch (const exception& e) {
        string msg = "Problem during plate::calculateCrust (width: ";
        msg = msg + Platec::to_string(width)
              + ", height: " + Platec::to_string(height)
              + ", x: " + Platec::to_string(x)
              + ", y:" + Platec::to_string(y) + ") :"
              + e.what();
        throw runtime_error(msg.c_str());
    }
}
