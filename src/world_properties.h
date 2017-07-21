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

#define NOMINMAX

class world_properties {
    const float_t float_min_value = 0.1f;
    const float_t max_continental_base = 3.f;
    const float_t max_noise_strength = 10.f;
    const float_t max_river_erosion_strength = 5.f;
    const float_t max_diffuse_strength = 15.f;

public:
    static world_properties& get();
    void setOceanic_base(float_t oceanic_base_) noexcept;
    float_t getOceanic_base() const noexcept;
    void setContinental_base(float_t continental_base_)noexcept;
    float_t getContinental_base() const noexcept;
    void setNoise_strength(float_t noise_strength_) noexcept;
    float_t getNoise_strength() const noexcept;
    void setNoise_enabel(bool noise_enabel_) noexcept;
    bool isNoise_enabel() const noexcept;
    void setRiver_erosion_enable(bool river_erosion_enable_) noexcept;
    bool isRiver_erosion_enable() const noexcept;
    void setRiver_erosion_strength(float_t river_erosion_strength_) noexcept;
    float_t getRiver_erosion_strength() const noexcept;
    void setNo_collision_time_limit(uint32_t no_collision_time_limit_)noexcept;
    uint32_t getNo_collision_time_limit() const noexcept;
    void setRestart_iterations(uint32_t restart_iterations_) noexcept;
    uint32_t getRestart_iterations() const noexcept;
    void setRestart_speed_limit(float_t restart_speed_limit_) noexcept;
    float_t getRestart_speed_limit() const noexcept;
    void setRestart_energy_ratio(float_t restart_energy_ratio_) noexcept;
    float_t getRestart_energy_ratio() const noexcept;
    void setMax_buoyancy_age(uint32_t max_buoyancy_age_) noexcept;
    uint32_t getMax_buoyancy_age() const noexcept;
    void setBuoyancy_bonus(float_t buoyancy_bonus_) noexcept;
    float_t getBuoyancy_bonus() const noexcept;
    void setSubduct_ratio(float_t subduct_ratio_) noexcept;
    float_t getSubduct_ratio() const noexcept;
    void setMax_plates(uint32_t max_plates_) noexcept;
    uint32_t getMax_plates() const noexcept;
    void setMax_cycles(uint32_t max_cycles_) noexcept;
    uint32_t getMax_cycles() const noexcept;
    void setFolding_ratio(float_t folding_ratio_) noexcept;
    float_t getFolding_ratio() const noexcept;
    void setErosion_period(uint32_t erosion_period_) noexcept;
    uint32_t getErosion_period() const noexcept;
    void setCycle_count(uint32_t cycle_count_) noexcept;
    uint32_t getCycle_count() const noexcept;
    void setAggr_overlap_rel(float_t aggr_overlap_rel_) noexcept;
    float_t getAggr_overlap_rel() const noexcept;
    void setAggr_overlap_abs(uint32_t aggr_overlap_abs_) noexcept;
    uint32_t getAggr_overlap_abs() const noexcept;
    void setWorldDimension(Dimension worldDimension_) noexcept;
    Dimension getWorldDimension() const noexcept;
    void setRegenerate_crust_enable(bool regenerate_crust_enable_) noexcept;
    bool isRegenerate_crust_enable() const noexcept;
    void setDiffuse_strength(float_t diffuse_strength_) noexcept;
    float_t getDiffuse_strength() const noexcept;

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
    float_t subduct_ratio = 0.5f;
    float_t buoyancy_bonus = 3.f;
    uint32_t max_buoyancy_age = 20u;
    float_t restart_energy_ratio = 0.15f;
    float_t restart_speed_limit = 2.0f;
    uint32_t restart_iterations = 600u;
    uint32_t no_collision_time_limit = 10u;
    float_t river_erosion_strength = 0.2f;
    bool river_erosion_enable = true;
    bool noise_enabel = true;
    bool regenerate_crust_enable = true;
    float_t noise_strength = 1.1f;
    float_t continental_base = 1.f;
    float_t oceanic_base = 0.1f;
    float_t diffuse_strength = 1.0f;
};

#endif /* WORLD_PROPERTIES_H */

