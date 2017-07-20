/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   world_properties.h
 * Author: ro-lip
 *
 * Created on 19. Juli 2017, 16:37
 */

#ifndef WORLD_PROPERTIES_H
#define WORLD_PROPERTIES_H

#include "dimension.h"


class world_properties {
public:
    world_properties& instance();

private:
    world_properties();
    //set Default values
    Dimension worldDimension = Dimension(0.0,0.0);
    uint32_t aggr_overlap_abs = 0u; ///< # of overlapping pixels -> aggregation.
    float_t  aggr_overlap_rel = 0.f; ///< % of overlapping area -> aggregation.
    uint32_t cycle_count = 0u; ///< Number of times the system's been restarted.
    uint32_t erosion_period = 0u; ///< # of iterations between global erosion.
    float_t  folding_ratio = 0.f; ///< Percent of overlapping crust that's folded.
    uint32_t max_cycles = 1u; ///< Max n:o of times the system'll be restarted.
    uint32_t max_plates = 1u; ///< Number of plates in the initial setting.
    float_t SUBDUCT_RATIO = 0.5f;
    float_t BUOYANCY_BONUS_X = 3.f;
    uint32_t MAX_BUOYANCY_AGE = 20u;
    float_t RESTART_ENERGY_RATIO = 0.15f;
    float_t RESTART_SPEED_LIMIT = 2.0f;
    uint32_t RESTART_ITERATIONS = 600u;
    uint32_t NO_COLLISION_TIME_LIMIT = 10u;
    float_t RIVER_EROSION_STRENGTH = 0.2f;
    bool RIVER_EROSION_ENABLE = true;
    bool NOISE_ENABEL = true;
    bool BOOL_REGENERATE_CRUST = true;
    float_t NOISE_STRENGTH = 1.1f;
    float_t CONTINATAL_BASE = 1.f;
    float_t OCEANIC_BASE = 0.1f;
};

#endif /* WORLD_PROPERTIES_H */

