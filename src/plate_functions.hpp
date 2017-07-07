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

#ifndef PLATE_FUNCTIONS_HPP
#define PLATE_FUNCTIONS_HPP

#include "utils.hpp"
#include "heightmap.hpp"
#include "bounds.hpp"

void calculateCrust(const Platec::vec2ui& position, const float_t height,
                    float& w_crust, float& e_crust, float& n_crust, float& s_crust,
                    uint32_t& w, uint32_t& e, uint32_t& n, uint32_t& s,
                    const Dimension& worldDimension,const HeightMap& map,
                    const Bounds& bounds);

#endif
