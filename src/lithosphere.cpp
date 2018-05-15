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

#include "lithosphere.hpp"
#include "plate.hpp"
#include "sqrdmd.hpp"
#include "simplexnoise.hpp"
#include "noise.hpp"
#include "world_properties.h"

#include <cfloat>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <numeric>




void lithosphere::createSlowNoise(float* tmp, const Dimension& tmpDim)
{
    ::createSlowNoise(tmp, tmpDim, _randsource);
}

lithosphere::lithosphere(long seed, uint32_t width, uint32_t height, float sea_level,
                         uint32_t erosion_period, float folding_ratio, uint32_t aggr_ratio_abs,
                         float aggr_ratio_rel, uint32_t num_cycles, uint32_t max_plates) :
    hmap(width, height),
    imap(width, height),        
    amap(width, height),
    plate_areas(max_plates),        
    plate_indices_found(max_plates),
    iter_count(0),
    num_plates(0),
    _randsource(seed),
    _steps(0),
        worldDimension(Dimension(width,height))
{
    //limit dimension

    world_properties::get().setWorldDimension(worldDimension);
    world_properties::get().setAggr_overlap_abs(aggr_ratio_abs);
    world_properties::get().setAggr_overlap_rel(aggr_ratio_rel);
    world_properties::get().setErosion_period(erosion_period);
    world_properties::get().setFolding_ratio(folding_ratio);
    world_properties::get().setMax_cycles(num_cycles);
    world_properties::get().setMax_plates(max_plates);
    Dimension tmpDim = Dimension(width+1, height+1);
    std::vector<float_t> tmp = std::vector<float_t>(tmpDim.getArea());

    createSlowNoise(&tmp[0], tmpDim);

    //find min and max element
    float_t lowest =  *std::min_element(tmp.begin(), tmp.end());
    float_t highest = *std::max_element(tmp.begin(), tmp.end());
    
    //normalize
    std::for_each(tmp.begin(),tmp.end(), [&](auto& value) {value = (value - lowest) / (highest - lowest);});

    float_t sea_threshold = 0.5;
    float_t th_step = 0.5;

    // Find the actual value in height map that produces the continent-sea
    // ratio defined be "sea_level".
    while (th_step > 0.01)
    {
        uint32_t count = std::count_if(tmp.begin(),tmp.end(), [&](auto& value){return value < sea_threshold;});

        th_step *= 0.5;
        if (count / (float_t)tmpDim.getArea() < sea_level)
            sea_threshold += th_step;
        else
            sea_threshold -= th_step;
    }

    sea_level = sea_threshold;
    
    // Genesis 1:9-10.
    std::for_each(tmp.begin(),tmp.end(), 
                [&](auto& value){ 
                    if(value > sea_level)
                     {
                        value += world_properties::get().getContinental_base();
                    }
                    else
                    {
                        value = world_properties::get().getOceanic_base();
                    }});


    // Scalp the +1 away from map side to get a power of two side length!
    // Practically only the redundant map edges become removed.

    std::copy_if(tmp.begin(),tmp.end(),hmap.getData().begin(),[&](auto)
    {
        static uint32_t counter = 0;
        ++counter;
        if(counter == worldDimension.getWidth())
        {
            counter = 0;
            return false;
        }
        return true;
    });               
                    

    collisions.resize(max_plates);
    subductions.resize(max_plates);

    // Create default plates
    createPlates();
}


void lithosphere::clearPlates() {

    plates.erase(plates.begin(),plates.end());
    num_plates = 0;
}

void lithosphere::growPlates()
{
    // "Grow" plates from their origins until surface is fully populated.
    uint32_t max_border = 1;
    uint32_t i;
    while (max_border) {
        for (max_border = i = 0; i < num_plates; ++i) {
            plateArea& area = plate_areas[i];
            const uint32_t N = (uint32_t)area.border.size();

            if (N == 0) {
                continue;
            }
            
            max_border = std::max(max_border , N);
            
            const uint32_t j = _randsource.next() % N;
            const auto p = worldDimension.coordOF(area.border[j]);

            const auto lft = worldDimension.xCap(p.getLeftPosition());
            const auto rgt = worldDimension.xMod(p.getRightPosition());
            const auto top =  worldDimension.yCap(p.getTopPosition() );
            const auto btm = worldDimension.yMod(p.getBottomPosition());

            const uint32_t n = worldDimension.indexOf(top); // North.
            const uint32_t s = worldDimension.indexOf(btm); // South.
            const uint32_t w = worldDimension.indexOf(lft);// West.
            const uint32_t e = worldDimension.indexOf(rgt);// East.

            if (imap[n] >= num_plates)
            {
                imap[n] = i;
                area.border.emplace_back(n);

                if (area.top == worldDimension.yMod(top.y() + 1))
                {
                    area.top = top.y();
                    ++area.hgt;
                }
            }

            if (imap[s] >= num_plates)
            {
                imap[s] = i;
                area.border.emplace_back(s);

                if (btm.y() == worldDimension.yMod(area.btm + 1))
                {
                    area.btm = btm.y();
                    ++area.hgt;
                }
            }

            if (imap[w] >= num_plates)
            {
                imap[w] = i;
                area.border.emplace_back(w);

                if (area.lft == worldDimension.xMod(lft.x() + 1))
                {
                    area.lft = lft.x();
                    ++area.wdt;
                }
            }

            if (imap[e] >= num_plates)
            {
                imap[e] = i;
                area.border.emplace_back(e);

                if (rgt.x() == worldDimension.xMod(area.rgt + 1))
                {
                    area.rgt = rgt.x();
                    ++area.wdt;
                }
            }

            // Overwrite processed point with unprocessed one.
            area.border[j] = area.border.back();
            area.border.pop_back();
        }
    }
}

void lithosphere::createPlates()
{
    try {
        const uint32_t map_area = worldDimension.getArea();
        num_plates = world_properties::get().getMax_plates();

        // Initialize "Free plate center position" lookup table.
        // This way two plate centers will never be identical.

        // Select N plate centers from the global map.
        std::vector<plateArea>(num_plates).swap(plate_areas);
        for (auto& area : plate_areas)
        {
            // Randomly select an unused plate origin.
            const uint32_t p = (uint32_t)_randsource.next() % (map_area);

            area.lft = area.rgt = worldDimension.xFromIndex(p); // Save origin...
            area.top = area.btm = worldDimension.yFromIndex(p);
            area.wdt = area.hgt = 1;

            area.border.clear();
            area.border.emplace_back(p); 
        }

        imap.set_all(std::numeric_limits<uint32_t>::max());

        growPlates();

        // check all the points of the map are owned
        for (const auto index : imap.getData()) {
            ASSERT(index<num_plates, "A point was not assigned to any plate");
        }
        plates = std::vector<std::unique_ptr<plate>>(num_plates);
        // Extract and create plates from initial terrain.
        for (uint32_t i = 0; i < num_plates; ++i) {
            plateArea& area = plate_areas[i];

            area.wdt = worldDimension.xCap(area.wdt);
            area.hgt = worldDimension.yCap(area.hgt);

            const uint32_t x0 = area.lft;
            const uint32_t x1 = 1 + x0 + area.wdt;
            const uint32_t y0 = area.top;
            const uint32_t y1 = 1 + y0 + area.hgt;
            const uint32_t width = x1 - x0;
            const uint32_t height = y1 - y0;
            auto pmap = std::vector<float_t>(width * height,0.f);
            auto pmapItr = pmap.begin();
            // Copy plate's height data from global map into local map.
            for (uint32_t y = y0; y < y1; ++y) 
            {
                for (uint32_t x = x0; x < x1; ++x) 
                {
                    auto k = worldDimension.pointMod(Platec::vec2ui(x, y));
                    if(imap[k] == i)
                    {
                        *pmapItr = hmap[k];
                    }
                    ++pmapItr;
                }
            }
            // Create plate.
            const Dimension plaDim(width, height);
            plates[i] = std::make_unique<plate>(_randsource.next(), HeightMap(std::move(pmap),plaDim)
                        , plaDim, Platec::vec2f(x0, y0), i);
        }

        iter_count = num_plates + world_properties::get().getMax_buoyancy_age();
        peak_Ek = 0;
        last_coll_count = 0;

    } catch (const std::exception& e) {
        std::string msg = "Problem during createPlates: ";
        msg = msg + e.what();
        throw std::runtime_error(msg.c_str());
    }
}

uint32_t lithosphere::getPlateCount() const
{
    return num_plates;
}

const uint32_t* lithosphere::getAgemap() const
{
    return amap.raw_data();
}

float* lithosphere::getTopography()
{
    return hmap.raw_data();
}

bool lithosphere::isFinished() const
{
    return getPlateCount() == 0;
}

// At least two plates are at same location.
// Move some crust from the SMALLER plate onto LARGER one.
void lithosphere::resolveJuxtapositions(std::unique_ptr<plate>& pla, const uint32_t ageMapValue, const float_t mapValue, 
                                        const Platec::vec2ui& p)
{
 
    // Record collisions to both plates. This also creates
    // continent segment at the collided location to plates.
    const uint32_t this_area = pla->addCollision(p);
    const uint32_t prev_area = plates[imap[p]]->addCollision(p);

    if (this_area < prev_area)
    {
        plateCollision coll(imap[p],p,
                            mapValue * world_properties::get().getFolding_ratio());

        // Give some...
        hmap[p] += coll.crust;
        plates[imap[p]]->setCrust(p, hmap[p], ageMapValue);

        // And take some.
        pla->setCrust(p, mapValue *
                            (1.0 - world_properties::get().getFolding_ratio()), ageMapValue);

        // Add collision to the earlier plate's list.
        collisions[pla->getIndex()].emplace_back(coll);
    }
    else
    {
        plateCollision coll(pla->getIndex(), p,
                            hmap[p] * world_properties::get().getFolding_ratio());

        pla->setCrust(p,mapValue+coll.crust, amap[p]);

        plates[imap[p]]->setCrust(p, hmap[p]
                                  * (1.0 - world_properties::get().getFolding_ratio()), amap[p]);

        collisions[imap[p]].emplace_back(coll);
        // Give the location to the larger plate.
        hmap[p] = mapValue;
        imap[p] = pla->getIndex();
        amap[p] = ageMapValue;
    }
}

// Update height and plate index maps.
// Doing it plate by plate is much faster than doing it index wise:
// Each plate's map's memory area is accessed sequentially and only
// once as opposed to calculating "num_plates" indices within plate
// maps in order to find out which plate(s) own current location.
uint32_t lithosphere::updateHeightAndPlateIndexMaps()
{
    uint32_t continental_collisions = 0;
    hmap.set_all(0);
    imap.set_all(std::numeric_limits<uint32_t>::max());
    for (auto& pla : plates)
    {
        const uint32_t x0 = pla->getLeftAsUint();
        const uint32_t y0 = pla->getTopAsUint();
        const uint32_t x1 = x0 + pla->getWidth();
        const uint32_t y1 = y0 + pla->getHeight();

        auto this_map = pla->getHeigthMap().getData().begin();
        auto this_age = pla->getAgeMap().getData().begin();
        

        // Copy first part of plate onto world map.
        for (uint32_t y = y0; y < y1; ++y)
        {
            for (uint32_t x = x0; x < x1; ++x, ++this_map,++this_age)
            {
                if (*this_map < 2 * FLT_EPSILON) // No crust here...
                    continue;
                const auto p = worldDimension.pointMod(Platec::vec2ui(x, y));

                if (imap[p] >= num_plates) // No one here yet?
                {
                    // This plate becomes the "owner" of current location
                    // if it is the first plate to have crust on it.
                    hmap[p] = *this_map;
                    imap[p] = pla->getIndex();
                    amap[p] = *this_age;

                    continue;
                }

                // DO NOT ACCEPT HEIGHT EQUALITY! Equality leads to subduction
                // of shore that 's barely above sea level. It's a lot less
                // serious problem to treat very shallow waters as continent...
                const bool prev_is_oceanic = hmap[p] < world_properties::get().getContinental_base();
                const bool this_is_oceanic = *this_map < world_properties::get().getContinental_base();

                const uint32_t prev_timestamp = plates[imap[p]]->
                                                getCrustTimestamp(p);
                const uint32_t this_timestamp = *this_age;
                const bool prev_is_bouyant = (hmap[p] > *this_map) |
                                                 ((hmap[p] + 2 * FLT_EPSILON > *this_map) &
                                                  (hmap[p] < 2 * FLT_EPSILON + *this_map) &
                                                  (prev_timestamp >= this_timestamp));

                // Handle subduction of oceanic crust as special case.
                if (this_is_oceanic && prev_is_bouyant) {
                    // This plate will be the subducting one.
                    // The level of effect that subduction has
                    // is directly related to the amount of water
                    // on top of the subducting plate.
                    const float sediment = world_properties::get().getSubduct_ratio() * world_properties::get().getOceanic_base() *
                                           (world_properties::get().getContinental_base() - *this_map) /
                                           world_properties::get().getContinental_base();

                    // Save collision to the receiving plate's list.
                    plateCollision coll(pla->getIndex(), p, sediment);
                    subductions[imap[p]].push_back(coll);

                    // Remove subducted oceanic lithosphere from plate.
                    // This is crucial for
                    // a) having correct amount of colliding crust (below)
                    // b) protecting subducted locations from receiving
                    //    crust from other subductions/collisions.
                    pla->setCrust(p, *this_map -
                                        world_properties::get().getOceanic_base() , this_timestamp);

                    if (*this_map <= 0)
                        continue; // Nothing more to collide.
                } else if (prev_is_oceanic) {
                    const float sediment = world_properties::get().getSubduct_ratio() * world_properties::get().getOceanic_base()  *
                                           (world_properties::get().getContinental_base() - hmap[p]) /
                                           world_properties::get().getContinental_base();

                    plateCollision coll(imap[p], p, sediment);
                    subductions[pla->getIndex()].emplace_back(coll);

                    plates[imap[p]]->setCrust(p, hmap[p] -
                                              world_properties::get().getOceanic_base()  , prev_timestamp);
                    hmap[p] -= world_properties::get().getOceanic_base()  ;

                    if (hmap[p] <= 0) {
                        imap[p] = pla->getIndex();
                        hmap[p] = *this_map;
                        amap[p] = *this_age;

                        continue;
                    }
                }

                resolveJuxtapositions(pla, *this_age,*this_map, p);
                ++continental_collisions;

            }
        }
    }
    return continental_collisions;
}

void lithosphere::updateCollisions()
{
    for (auto& pla : plates)
    {
        for (const auto& coll : collisions[pla->getIndex()])
        {
            uint32_t coll_count, coll_count_i, coll_count_j;
            float coll_ratio, coll_ratio_i, coll_ratio_j;

            ASSERT(pla->getIndex() != coll.index, "when colliding: SRC == DEST!");

            // Collision causes friction. Apply it to both plates.
            pla->applyFriction(coll.crust);
            plates[coll.index]->applyFriction(coll.crust);

            auto pair1 = pla->getCollisionInfo(coll.point);
            coll_count_i = pair1.first;
            coll_ratio_i = pair1.second;
            auto pair2 = plates[coll.index]->getCollisionInfo(coll.point);
            coll_count_j = pair2.first;
            coll_ratio_j = pair2.second;
            // Find the minimum count of collisions between two
            // continents on different plates.
            // It's minimum because large plate will get collisions
            // from all over where as smaller plate will get just
            // a few. It's those few that matter between these two
            // plates, not what the big plate has with all the
            // other plates around it.
            coll_count = coll_count_i;
            if(coll_count > coll_count_j)
            {
                coll_count -= (coll_count - coll_count_j);
            }

            // Find maximum amount of collided surface area between
            // two continents on different plates.
            // Like earlier, it's the "experience" of the smaller
            // plate that matters here.
            coll_ratio = coll_ratio_i;
            if(coll_ratio_j > coll_ratio)
            {
                coll_ratio += (coll_ratio_j - coll_ratio);
            }

            if ((coll_count > world_properties::get().getAggr_overlap_abs()) ||
                    (coll_ratio > world_properties::get().getAggr_overlap_rel()))
            {
                float amount = pla->aggregateCrust(
                                   *plates[coll.index],
                                   coll.point);

                // Calculate new direction and speed for the
                // merged plate system, that is, for the
                // receiving plate!
                plates[coll.index]->collide(*pla, amount);
            }
        }

        collisions[pla->getIndex()].clear();
    }
}

// Remove empty plates from the system.
void lithosphere::removeEmptyPlates()
{
    if (num_plates > 1)
    {
        for (auto& pla : plates)
        {
            if (plate_indices_found[pla->getIndex()] == 0)
            {
                pla = std::move(plates[num_plates - 1]);
                plates.erase(plates.end() -1);
                pla->setIndex(num_plates - 1);
                plate_indices_found[pla->getIndex()]= std::move(plate_indices_found[num_plates - 1]);
                plate_indices_found.erase(plate_indices_found.end() -1);
                // Life is seldom as simple as seems at first.
                // Replace the moved plate's index in the index map
                // to match its current position in the array!
                --num_plates;
                std::replace_if(imap.getData().begin(),imap.getData().end(),
                        [&](const auto& val){return val == num_plates;},pla->getIndex()
                      );
            }
        }
    }
    else
    {
         puts("ONLY ONE PLATE LEFT!");
    }
}

void lithosphere::update()
{
    try {
        ++_steps;
        float totalVelocity = std::accumulate(plates.begin(),plates.end(),0.f,[&](float sum, auto& plate){return sum + plate->getVelocity();} );;
        float systemKineticEnergy = std::accumulate(plates.begin(),plates.end(),0.f,[&](float sum, auto& plate){return sum + plate->getMomentum();} );
        

        peak_Ek = std::max(systemKineticEnergy,peak_Ek);
        

        // If there's no continental collisions during past iterations,
        // then interesting activity has ceased and we should restart.
        // Also if the simulation has been going on for too long already,
        // restart, because interesting stuff has most likely ended.
        if (totalVelocity < world_properties::get().getRestart_speed_limit() ||
                systemKineticEnergy / peak_Ek < world_properties::get().getRestart_energy_ratio() ||
                last_coll_count > world_properties::get().getNo_collision_time_limit() ||
                iter_count > world_properties::get().getRestart_iterations())
        {
            restart();
            return;
        }

        // Keep a copy of the previous index map
        IndexMap prev_imap = imap;

        // Realize accumulated external forces to each plate.
        for (auto& val : plates)
        {
            val->resetSegments();
            val->move();
        }
        if (world_properties::get().getErosion_period() > 0 && iter_count % world_properties::get().getErosion_period() == 0)
        {
            for (auto& val : plates)
            {
                val->erode(world_properties::get().getContinental_base());
             
            }       
        }
                
        // Update the counter of iterations since last continental collision.
        if(updateHeightAndPlateIndexMaps() == 0)
        {
            ++last_coll_count;
        }

        for (auto& val : plates)
        {
            for (const auto& coll : subductions[val->getIndex()])
            {
                ASSERT(val->getIndex() != coll.index, "when subducting: SRC == DEST!");

                // Do not apply friction to oceanic plates.
                // This is a very cheap way to emulate slab pull.
                // Just perform subduction and on our way we go!
                
                val->addCrustBySubduction(
                    coll.point, coll.crust, iter_count,
                  plates[coll.index]->getVelocityVector());
            }

            subductions[val->getIndex()].clear();
        }

        updateCollisions();

        std::fill(plate_indices_found.begin(), plate_indices_found.end(), 0);
        
        if(world_properties::get().isRegenerate_crust_enable())
        {
            auto amapPrt = amap.getData().begin();
            auto hmapPtr = hmap.getData().begin();
            std::for_each(imap.getData().begin(),imap.getData().end(),
            [&](const auto& index) 
            {
                if(index >= num_plates) 
                {
                    *amapPrt = iter_count;
                    *hmapPtr = world_properties::get().getOceanic_base() * world_properties::get().getBuoyancy_bonus();
                }
                ++amapPrt;
                ++hmapPtr;
            });
            
            uint32_t i = 0;
            auto prevImapItr = prev_imap.getData().begin();
            std::for_each(imap.getData().begin(), imap.getData().end(),
                    [&](auto& val)
                    {
                        if(val >= num_plates)
                        {
                            val = *prevImapItr;
                            if(val < num_plates)
                            {
                                plates[val]->setCrust(worldDimension.coordOF(i), world_properties::get().getOceanic_base(),
                                                  iter_count);
                            }
                        }
                        ++i;
                        ++prevImapItr;
                    }
                    );
            std::for_each(imap.getData().begin(), imap.getData().end(),
                    [&](auto& val) {
                        if(val < num_plates)
                        {
                            ++plate_indices_found[val];
                        }
                    });      
            std::replace_if(hmap.getData().begin(), hmap.getData().end(), [&](const auto& h)
                    {return h <= 0;},2 * FLT_EPSILON);
            
        }


        removeEmptyPlates();


        // Add some "virginity buoyancy" to all pixels for a visual boost! :)
        if(world_properties::get().getBuoyancy_bonus() > 0)
        {
            auto ageItr = amap.getData().begin();
            for (auto& val : hmap.getData())
            {
                 // Calculate the inverted age of this piece of crust.
                // Force result to be minimum between inv. age and
                // max buoyancy bonus age.
                if(val < world_properties::get().getContinental_base())
                {
                    uint32_t crust_age = iter_count - *ageItr;
                    crust_age = world_properties::get().getMax_buoyancy_age() - crust_age;
                    if(crust_age <= world_properties::get().getMax_buoyancy_age())
                    {
                        val += world_properties::get().getBuoyancy_bonus() *
                                   world_properties::get().getOceanic_base() * crust_age * (1.f/world_properties::get().getMax_buoyancy_age());  
                    }
                }
                ++ageItr;
            }
        }

        ++iter_count;
    } catch (const std::exception& e) {
        std::string msg = "Problem during update: ";
        msg = msg + e.what();
        std::cerr << msg << std::endl;
        throw std::runtime_error(msg.c_str());
    }
}

void lithosphere::restart()
{
    try {

        const uint32_t map_area = worldDimension.getArea();
        if(world_properties::get().getMax_cycles() != 0)
        {
            world_properties::get().setCycle_count(world_properties::get().getCycle_count() +1); // No increment if running for ever.
            if (world_properties::get().getCycle_count() > world_properties::get().getMax_cycles())
                return;
        }

        // Update height map to include all recent changes.
        hmap.set_all(0);
        for (auto& pla : plates)
        {
            const uint32_t x0 = pla->getLeftAsUint();
            const uint32_t y0 = pla->getTopAsUint();
            const uint32_t x1 = x0 + pla->getWidth();
            const uint32_t y1 = y0 + pla->getHeight();

            const HeightMap& this_map = pla->getHeigthMap();
            const AgeMap& this_age = pla->getAgeMap();

            // Copy first part of plate onto world map.
            for (uint32_t y = y0, j = 0; y < y1; ++y)
            {
                for (uint32_t x = x0; x < x1; ++x, ++j)
                {
                    const auto index = worldDimension.normalize(
                                        Platec::vec2ui(x,y));

                    const float h0 = hmap[index];
                    const float h1 = this_map[j];
                    const uint32_t a0 = amap[index];
                    const uint32_t a1 =  this_age[j];

                    amap[index] = (h0 *a0 +h1 *a1) /(h0 +h1);
                    hmap[index] += this_map[j];
                }
            }
        }
        // Clear plate array
        clearPlates();

        // create new plates IFF there are cycles left to run!
        // However, if max cycle count is "ETERNITY", then 0 < 0 + 1 always.
        
        if (world_properties::get().getCycle_count() < world_properties::get().getMax_cycles() + !world_properties::get().getMax_cycles())
        {
            createPlates();

            // Restore the ages of plates' points of crust!
            for (auto& pla : plates)
            {
                const uint32_t x0 = pla->getLeftAsUint();
                const uint32_t y0 = pla->getTopAsUint();
                const uint32_t x1 = x0 + pla->getWidth();
                const uint32_t y1 = y0 + pla->getHeight();
                
                auto this_age = pla->getAgeMap().getData().begin();

                for (uint32_t y = y0; y < y1; ++y)
                {
                    for (uint32_t x = x0; x < x1; ++x)
                    {
                        *this_age = amap[worldDimension.normalize(Platec::vec2ui(x,y))];
                        ++this_age;
                    }
                }
            }

            return;
        }

        // Add some "virginity buoyancy" to all pixels for a visual boost.
        if(world_properties::get().getBuoyancy_bonus() > 0)
        {
            for (uint32_t i = 0; i <  map_area; ++i)
            {
                if(hmap[i] < world_properties::get().getContinental_base())
                {
                    uint32_t crust_age = iter_count - amap[i];
                    crust_age = world_properties::get().getMax_buoyancy_age() - crust_age;
                    crust_age &= -(crust_age <= world_properties::get().getMax_buoyancy_age());
                    hmap[i] += world_properties::get().getBuoyancy_bonus() *
                               world_properties::get().getOceanic_base() * crust_age * (1.f/world_properties::get().getMax_buoyancy_age());
                }
            }
        }
    } catch (const std::exception& e) {
        std::string msg = "Problem during restart: ";
        msg = msg + e.what();
        throw std::runtime_error(msg.c_str());
    }
}

uint32_t lithosphere::getWidth() const
{
    return worldDimension.getWidth();
}

uint32_t lithosphere::getHeight() const
{
    return worldDimension.getHeight();
}

uint32_t* lithosphere::getPlatesMap(){
    return imap.raw_data();
}

const plate* lithosphere::getPlate(uint32_t index) const
{
    ASSERT(index < num_plates, "invalid plate index");
    return plates[index].get();
}
