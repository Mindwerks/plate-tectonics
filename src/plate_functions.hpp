#ifndef PLATE_FUNCTIONS_HPP
#define PLATE_FUNCTIONS_HPP

#include "utils.hpp"
#include "rectangle.hpp"
#include "heightmap.hpp"

void calculateCrust(uint32_t x, uint32_t y, uint32_t index, 
        float& w_crust, float& e_crust, float& n_crust, float& s_crust,
        uint32_t& w, uint32_t& e, uint32_t& n, uint32_t& s,
        const WorldDimension& worldDimension, HeightMap& map,
        const uint32_t width, const uint32_t height);

#endif
