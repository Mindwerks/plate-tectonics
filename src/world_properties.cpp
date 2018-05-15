/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   world_properties.cpp
 * Author: ro-lip
 * 
 * Created on 19. Juli 2017, 16:37
 */

#include "world_properties.h"

world_properties::world_properties() {
}

world_properties& world_properties::get() {
   static world_properties instance;
   return instance;
}

void world_properties::setOceanic_base(float_t oceanic_base_) noexcept {
    this->oceanic_base = std::max(float_min_value, std::min(oceanic_base_, continental_base-0.1f));
}

float_t world_properties::getOceanic_base() const noexcept {
    return oceanic_base;
}

void world_properties::setContinental_base(float_t continental_base_) noexcept {
    this->continental_base = std::max(oceanic_base, std::min(continental_base_, max_continental_base));
}

float_t world_properties::getContinental_base() const noexcept {
    return continental_base;
}

void world_properties::setNoise_strength(float_t noise_strength_) noexcept {
    this->noise_strength = std::max(0.0f, std::min(noise_strength_, max_noise_strength));
}

float_t world_properties::getNoise_strength() const noexcept {
    return noise_strength;
}


void world_properties::setNoise_enabel(bool noise_enabel_) noexcept {
    this->noise_enabel = noise_enabel_;
}

bool world_properties::isNoise_enabel() const noexcept {
    return noise_enabel;
}

void world_properties::setRiver_erosion_enable(bool river_erosion_enable_) noexcept {
    this->river_erosion_enable = river_erosion_enable_;
}

bool world_properties::isRiver_erosion_enable() const noexcept {
    return river_erosion_enable;
}

void world_properties::setRiver_erosion_strength(float_t river_erosion_strength_) noexcept {
    this->river_erosion_strength = std::max(0.0f, std::min(river_erosion_strength_, max_river_erosion_strength));
}

float_t world_properties::getRiver_erosion_strength() const noexcept {
    return river_erosion_strength;
}

void world_properties::setNo_collision_time_limit(uint32_t no_collision_time_limit_) noexcept {
    this->no_collision_time_limit = no_collision_time_limit_;
}

uint32_t world_properties::getNo_collision_time_limit() const noexcept {
    return no_collision_time_limit;
}

void world_properties::setRestart_iterations(uint32_t restart_iterations_) noexcept {
    this->restart_iterations = restart_iterations_;
}

uint32_t world_properties::getRestart_iterations() const noexcept {
    return restart_iterations;
}

void world_properties::setRestart_speed_limit(float_t restart_speed_limit_) noexcept {
    this->restart_speed_limit = std::max(float_min_value,restart_speed_limit_);
}

float_t world_properties::getRestart_speed_limit() const noexcept {
    return restart_speed_limit;
}

void world_properties::setRestart_energy_ratio(float_t restart_energy_ratio_) noexcept {
    this->restart_energy_ratio = std::max(0.0f,restart_energy_ratio_);
}

float_t world_properties::getRestart_energy_ratio() const noexcept {
    return restart_energy_ratio;
}

void world_properties::setMax_buoyancy_age(uint32_t max_buoyancy_age_) noexcept {
    this->max_buoyancy_age = max_buoyancy_age_;
}

uint32_t world_properties::getMax_buoyancy_age() const noexcept {
    return max_buoyancy_age;
}

void world_properties::setBuoyancy_bonus(float_t buoyancy_bonus_) noexcept {
    this->buoyancy_bonus = std::max(0.0f,buoyancy_bonus_);
}

float_t world_properties::getBuoyancy_bonus() const noexcept {
    return buoyancy_bonus;
}

void world_properties::setSubduct_ratio(float_t subduct_ratio_) noexcept {
    this->subduct_ratio = std::max(0.0f,subduct_ratio_);
}

float_t world_properties::getSubduct_ratio() const noexcept {
    return subduct_ratio;
}

void world_properties::setMax_plates(uint32_t max_plates_) noexcept {
    this->max_plates = std::max(1u,max_plates_);
}

uint32_t world_properties::getMax_plates() const noexcept {
    return max_plates;
}

void world_properties::setMax_cycles(uint32_t max_cycles_) noexcept {
    this->max_cycles = max_cycles_;
}

uint32_t world_properties::getMax_cycles() const noexcept {
    return max_cycles;
}

void world_properties::setFolding_ratio(float_t folding_ratio_) noexcept {
    this->folding_ratio = std::max(0.0f,folding_ratio_);
}

float_t world_properties::getFolding_ratio() const noexcept {
    return folding_ratio;
}

void world_properties::setErosion_period(uint32_t erosion_period_) noexcept {
    this->erosion_period = erosion_period_;
}

uint32_t world_properties::getErosion_period() const noexcept {
    return erosion_period;
}

void world_properties::setCycle_count(uint32_t cycle_count_) noexcept {
    this->cycle_count = cycle_count_;
}

uint32_t world_properties::getCycle_count() const noexcept {
    return cycle_count;
}

void world_properties::setAggr_overlap_rel(float_t aggr_overlap_rel_) noexcept {
    this->aggr_overlap_rel = std::max(float_min_value,aggr_overlap_rel_);
}

float_t world_properties::getAggr_overlap_rel() const noexcept {
    return aggr_overlap_rel;
}

void world_properties::setAggr_overlap_abs(uint32_t aggr_overlap_abs_) noexcept {
    this->aggr_overlap_abs = aggr_overlap_abs_;
}

uint32_t world_properties::getAggr_overlap_abs() const noexcept {
    return aggr_overlap_abs;
}

void world_properties::setWorldDimension(Dimension worldDimension_) noexcept {
    this->worldDimension = Dimension(std::max(5u,worldDimension_.getWidth()),
                                      std::max(5u,worldDimension_.getHeight()));}

const Dimension& world_properties::getWorldDimension() const noexcept {
    return worldDimension;
}

void world_properties::setRegenerate_crust_enable(bool regenerate_crust_enable_) noexcept {
    this->regenerate_crust_enable = regenerate_crust_enable_;
}

bool world_properties::isRegenerate_crust_enable() const noexcept {
    return regenerate_crust_enable;
}

void world_properties::setDiffuse_strength(float_t diffuse_strength_) noexcept {
    this->diffuse_strength = std::max(0.0f, std::min(diffuse_strength_, max_diffuse_strength));
}

float_t world_properties::getDiffuse_strength() const noexcept{
    return diffuse_strength;
}
