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

#include <cfloat>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <iostream>

#define BOOL_REGENERATE_CRUST   1

using namespace std;

static const float SUBDUCT_RATIO = 0.5f;

static const float BUOYANCY_BONUS_X = 3;
static const uint32_t MAX_BUOYANCY_AGE = 20;
static const float MULINV_MAX_BUOYANCY_AGE = 1.0f / (float)MAX_BUOYANCY_AGE;

static const float RESTART_ENERGY_RATIO = 0.15;
static const float RESTART_SPEED_LIMIT = 2.0;
static const uint32_t RESTART_ITERATIONS = 600;
static const uint32_t NO_COLLISION_TIME_LIMIT = 10;

uint32_t findBound(const uint32_t* map, uint32_t length, uint32_t x0, uint32_t y0,
                   int dx, int dy);
uint32_t findPlate(plate** plates, float x, float y, uint32_t num_plates);

WorldPoint lithosphere::randomPosition()
{
    return WorldPoint(
               _randsource.next() % _worldDimension.getWidth(),
               _randsource.next() % _worldDimension.getHeight(),
               _worldDimension);
}

void lithosphere::createNoise(float* tmp, const WorldDimension& tmpDim, bool useSimplex)
{
    ::createNoise(tmp, tmpDim, _randsource, useSimplex);
}

void lithosphere::createSlowNoise(float* tmp, const WorldDimension& tmpDim)
{
    ::createSlowNoise(tmp, tmpDim, _randsource);
}

lithosphere::lithosphere(long seed, uint32_t width, uint32_t height, float sea_level,
                         uint32_t _erosion_period, float _folding_ratio, uint32_t aggr_ratio_abs,
                         float aggr_ratio_rel, uint32_t num_cycles, uint32_t _max_plates) noexcept(false) :
    hmap(width, height),
    amap(width, height),
    imap(width, height),
    prev_imap(width, height),
    plates(nullptr),
    plate_indices_found(_max_plates),
    plate_areas(_max_plates),
    aggr_overlap_abs(aggr_ratio_abs),
    aggr_overlap_rel(aggr_ratio_rel),
    cycle_count(0),
    erosion_period(_erosion_period),
    folding_ratio(_folding_ratio),
    iter_count(0),
    max_cycles(num_cycles),
    max_plates(_max_plates),
    num_plates(0),
    _worldDimension(width, height),
    _randsource(seed),
    _steps(0)
{
    if (width < 5 || height < 5) {
        throw runtime_error("Width and height should be >=5");
    }

    WorldDimension tmpDim = WorldDimension(width+1, height+1);
    const uint32_t A = tmpDim.getArea();
    float* tmp = new float[A];

    createSlowNoise(tmp, tmpDim);

    float lowest = tmp[0], highest = tmp[0];
    for (uint32_t i = 1; i < A; ++i)
    {
        lowest = lowest < tmp[i] ? lowest : tmp[i];
        highest = highest > tmp[i] ? highest : tmp[i];
    }

    for (uint32_t i = 0; i < A; ++i) // Scale to [0 ... 1]
        tmp[i] = (tmp[i] - lowest) / (highest - lowest);

    float sea_threshold = 0.5;
    float th_step = 0.5;

    // Find the actual value in height map that produces the continent-sea
    // ratio defined be "sea_level".
    while (th_step > 0.01)
    {
        uint32_t count = 0;
        for (uint32_t i = 0; i < A; ++i)
            count += (tmp[i] < sea_threshold);

        th_step *= 0.5;
        if (count / (float)A < sea_level)
            sea_threshold += th_step;
        else
            sea_threshold -= th_step;
    }

    sea_level = sea_threshold;
    for (uint32_t i = 0; i < A; ++i) // Genesis 1:9-10.
    {
        tmp[i] = (tmp[i] > sea_level) *
                 (tmp[i] + CONTINENTAL_BASE) +
                 (tmp[i] <= sea_level) * OCEANIC_BASE;
    }

    // Scalp the +1 away from map side to get a power of two side length!
    // Practically only the redundant map edges become removed.
    for (uint32_t y = 0; y < _worldDimension.getHeight(); ++y) {
        memcpy(&hmap[_worldDimension.lineIndex(y)],
               &tmp[ tmpDim.lineIndex(y)],
               _worldDimension.getWidth()*sizeof(float));
    }

    delete[] tmp;

    collisions.resize(max_plates);
    subductions.resize(max_plates);

    // Create default plates
    plates = new plate*[max_plates];
    for (uint32_t i = 0; i < max_plates; i++) {
        plate_areas[i].border.reserve(8);
    }
    createPlates();
}

lithosphere::~lithosphere() throw()
{
    clearPlates();
    delete[] plates;
    plates = 0;
}

void lithosphere::clearPlates() {
    for (uint32_t i = 0; i < num_plates; i++) {
        delete plates[i];
    }
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
            max_border = max_border > N ? max_border : N;

            if (N == 0) {
                continue;
            }
            const uint32_t j = _randsource.next() % N;
            const uint32_t p = area.border[j];
            const uint32_t cy = _worldDimension.yFromIndex(p);
            const uint32_t cx = _worldDimension.xFromIndex(p);

            const uint32_t lft = cx > 0 ? cx - 1 : _worldDimension.getWidth() - 1;
            const uint32_t rgt = cx < _worldDimension.getWidth() - 1 ? cx + 1 : 0;
            const uint32_t top = cy > 0 ? cy - 1 : _worldDimension.getHeight() - 1;
            const uint32_t btm = cy < _worldDimension.getHeight() - 1 ? cy + 1 : 0;

            const uint32_t n = top * _worldDimension.getWidth() +  cx; // North.
            const uint32_t s = btm * _worldDimension.getWidth() +  cx; // South.
            const uint32_t w =  cy * _worldDimension.getWidth() + lft; // West.
            const uint32_t e =  cy * _worldDimension.getWidth() + rgt; // East.

            if (imap[n] >= num_plates)
            {
                imap[n] = i;
                area.border.push_back(n);

                if (area.top == _worldDimension.yMod(top + 1))
                {
                    area.top = top;
                    area.hgt++;
                }
            }

            if (imap[s] >= num_plates)
            {
                imap[s] = i;
                area.border.push_back(s);

                if (btm == _worldDimension.yMod(area.btm + 1))
                {
                    area.btm = btm;
                    area.hgt++;
                }
            }

            if (imap[w] >= num_plates)
            {
                imap[w] = i;
                area.border.push_back(w);

                if (area.lft == _worldDimension.xMod(lft + 1))
                {
                    area.lft = lft;
                    area.wdt++;
                }
            }

            if (imap[e] >= num_plates)
            {
                imap[e] = i;
                area.border.push_back(e);

                if (rgt == _worldDimension.xMod(area.rgt + 1))
                {
                    area.rgt = rgt;
                    area.wdt++;
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
        const uint32_t map_area = _worldDimension.getArea();
        num_plates = max_plates;

        // Initialize "Free plate center position" lookup table.
        // This way two plate centers will never be identical.
        for (uint32_t i = 0; i < map_area; ++i)
            imap[i] = i;

        // Select N plate centers from the global map.

        for (uint32_t i = 0; i < num_plates; ++i)
        {
            plateArea& area = plate_areas[i];

            // Randomly select an unused plate origin.
            const uint32_t p = imap[(uint32_t)_randsource.next() % (map_area - i)];
            const uint32_t y = _worldDimension.yFromIndex(p);
            const uint32_t x = _worldDimension.xFromIndex(p);

            area.lft = area.rgt = x; // Save origin...
            area.top = area.btm = y;
            area.wdt = area.hgt = 1;

            area.border.clear();
            area.border.push_back(p); // ...and mark it as border.

            // Overwrite used entry with last unused entry in array.
            imap[p] = imap[map_area - i - 1];
        }

        imap.set_all(0xFFFFFFFF);

        growPlates();

        // check all the points of the map are owned
        for (int i=0; i < map_area; i++) {
            ASSERT(imap[i]<num_plates, "A point was not assigned to any plate");
        }

        // Extract and create plates from initial terrain.
        for (uint32_t i = 0; i < num_plates; ++i) {
            plateArea& area = plate_areas[i];

            area.wdt = _worldDimension.xCap(area.wdt);
            area.hgt = _worldDimension.yCap(area.hgt);

            const uint32_t x0 = area.lft;
            const uint32_t x1 = 1 + x0 + area.wdt;
            const uint32_t y0 = area.top;
            const uint32_t y1 = 1 + y0 + area.hgt;
            const uint32_t width = x1 - x0;
            const uint32_t height = y1 - y0;
            float* pmap = new float[width * height];

            // Copy plate's height data from global map into local map.
            for (uint32_t y = y0, j = 0; y < y1; ++y) {
                for (uint32_t x = x0; x < x1; ++x, ++j) {
                    uint32_t k = _worldDimension.normalizedIndexOf(x, y);
                    pmap[j] = hmap[k] * (imap[k] == i);
                }
            }
            // Create plate.
            // MK: The pmap array becomes owned by map, do not delete it
            plates[i] = new plate(_randsource.next(), pmap, width, height, x0, y0, i, _worldDimension);
        }

        iter_count = num_plates + MAX_BUOYANCY_AGE;
        peak_Ek = 0;
        last_coll_count = 0;

    } catch (const exception& e) {
        string msg = "Problem during createPlates: ";
        msg = msg + e.what();
        throw runtime_error(msg.c_str());
    }
}

uint32_t lithosphere::getPlateCount() const throw()
{
    return num_plates;
}

const uint32_t* lithosphere::getAgeMap() const throw()
{
    return amap.raw_data();
}

float* lithosphere::getTopography() const throw()
{
    return hmap.raw_data();
}

bool lithosphere::isFinished() const
{
    return getPlateCount() == 0;
}

// At least two plates are at same location.
// Move some crust from the SMALLER plate onto LARGER one.
void lithosphere::resolveJuxtapositions(const uint32_t& i, const uint32_t& j, const uint32_t& k,
                                        const uint32_t& x_mod, const uint32_t& y_mod,
                                        const float*& this_map, const uint32_t*& this_age, uint32_t& continental_collisions)
{
    ASSERT(i<num_plates, "Given invalid plate index");

    // Record collisions to both plates. This also creates
    // continent segment at the collided location to plates.
    uint32_t this_area = plates[i]->addCollision(x_mod, y_mod);
    uint32_t prev_area = plates[imap[k]]->addCollision(x_mod, y_mod);

    if (this_area < prev_area)
    {
        plateCollision coll(imap[k], x_mod, y_mod,
                            this_map[j] * folding_ratio);

        // Give some...
        hmap[k] += coll.crust;
        plates[imap[k]]->setCrust(x_mod, y_mod, hmap[k],
                                  this_age[j]);

        // And take some.
        plates[i]->setCrust(x_mod, y_mod, this_map[j] *
                            (1.0 - folding_ratio), this_age[j]);

        // Add collision to the earlier plate's list.
        collisions[i].push_back(coll);
        ++continental_collisions;
    }
    else
    {
        plateCollision coll(i, x_mod, y_mod,
                            hmap[k] * folding_ratio);

        plates[i]->setCrust(x_mod, y_mod,
                            this_map[j]+coll.crust, amap[k]);

        plates[imap[k]]->setCrust(x_mod, y_mod, hmap[k]
                                  * (1.0 - folding_ratio), amap[k]);

        collisions[imap[k]].push_back(coll);
        ++continental_collisions;

        // Give the location to the larger plate.
        hmap[k] = this_map[j];
        imap[k] = i;
        amap[k] = this_age[j];
    }
}

// Update height and plate index maps.
// Doing it plate by plate is much faster than doing it index wise:
// Each plate's map's memory area is accessed sequentially and only
// once as opposed to calculating "num_plates" indices within plate
// maps in order to find out which plate(s) own current location.
void lithosphere::updateHeightAndPlateIndexMaps(const uint32_t& map_area,
        uint32_t& oceanic_collisions,
        uint32_t& continental_collisions)
{
    uint32_t world_width = _worldDimension.getWidth();
    uint32_t world_height = _worldDimension.getHeight();
    hmap.set_all(0);
    imap.set_all(0xFFFFFFFF);
    for (uint32_t i = 0; i < num_plates; ++i)
    {
        const uint32_t x0 = plates[i]->getLeftAsUint();
        const uint32_t y0 = plates[i]->getTopAsUint();
        const uint32_t x1 = x0 + plates[i]->getWidth();
        const uint32_t y1 = y0 + plates[i]->getHeight();

        const float*  this_map;
        const uint32_t* this_age;
        plates[i]->getMap(&this_map, &this_age);

        uint32_t x_mod_start = (x0 + world_width) % world_width;
        uint32_t y_mod = (y0 + world_height) % world_height;

        // Copy first part of plate onto world map.
        // MK: These loops are ugly, but using modulus in here is a hog
        for (uint32_t y = y0, j = 0; y < y1; ++y,
                y_mod = ++y_mod >= world_height ? y_mod - world_height : y_mod)
        {
            const uint32_t y_width = y_mod * world_width;
            uint32_t x_mod = x_mod_start;

            for (uint32_t x = x0; x < x1; ++x, ++j,
                    x_mod = ++x_mod >= world_width ? x_mod - world_width : x_mod)
            {
                const uint32_t k = x_mod + y_width;

                if (this_map[j] < 2 * FLT_EPSILON) // No crust here...
                    continue;

                if (imap[k] >= num_plates) // No one here yet?
                {
                    // This plate becomes the "owner" of current location
                    // if it is the first plate to have crust on it.
                    hmap[k] = this_map[j];
                    imap[k] = i;
                    amap[k] = this_age[j];

                    continue;
                }

                // DO NOT ACCEPT HEIGHT EQUALITY! Equality leads to subduction
                // of shore that 's barely above sea level. It's a lot less
                // serious problem to treat very shallow waters as continent...
                const bool prev_is_oceanic = hmap[k] < CONTINENTAL_BASE;
                const bool this_is_oceanic = this_map[j] < CONTINENTAL_BASE;

                const uint32_t prev_timestamp = plates[imap[k]]->
                                                getCrustTimestamp(x_mod, y_mod);
                const uint32_t this_timestamp = this_age[j];
                const uint32_t prev_is_buoyant = (hmap[k] > this_map[j]) |
                                                 ((hmap[k] + 2 * FLT_EPSILON > this_map[j]) &
                                                  (hmap[k] < 2 * FLT_EPSILON + this_map[j]) &
                                                  (prev_timestamp >= this_timestamp));

                // Handle subduction of oceanic crust as special case.
                if (this_is_oceanic & prev_is_buoyant) {
                    // This plate will be the subducting one.
                    // The level of effect that subduction has
                    // is directly related to the amount of water
                    // on top of the subducting plate.
                    const float sediment = SUBDUCT_RATIO * OCEANIC_BASE *
                                           (CONTINENTAL_BASE - this_map[j]) /
                                           CONTINENTAL_BASE;

                    // Save collision to the receiving plate's list.
                    plateCollision coll(i, x_mod, y_mod, sediment);
                    subductions[imap[k]].push_back(coll);
                    ++oceanic_collisions;

                    // Remove subducted oceanic lithosphere from plate.
                    // This is crucial for
                    // a) having correct amount of colliding crust (below)
                    // b) protecting subducted locations from receiving
                    //    crust from other subductions/collisions.
                    plates[i]->setCrust(x_mod, y_mod, this_map[j] -
                                        OCEANIC_BASE, this_timestamp);

                    if (this_map[j] <= 0)
                        continue; // Nothing more to collide.
                } else if (prev_is_oceanic) {
                    const float sediment = SUBDUCT_RATIO * OCEANIC_BASE *
                                           (CONTINENTAL_BASE - hmap[k]) /
                                           CONTINENTAL_BASE;

                    plateCollision coll(imap[k], x_mod, y_mod, sediment);
                    subductions[i].push_back(coll);
                    ++oceanic_collisions;

                    plates[imap[k]]->setCrust(x_mod, y_mod, hmap[k] -
                                              OCEANIC_BASE, prev_timestamp);
                    hmap[k] -= OCEANIC_BASE;

                    if (hmap[k] <= 0) {
                        imap[k] = i;
                        hmap[k] = this_map[j];
                        amap[k] = this_age[j];

                        continue;
                    }
                }

                resolveJuxtapositions(i, j, k, x_mod, y_mod,
                                      this_map, this_age, continental_collisions);
            }
        }
    }
}

void lithosphere::updateCollisions()
{
    for (uint32_t i = 0; i < num_plates; ++i)
    {
        for (uint32_t j = 0; j < collisions[i].size(); ++j)
        {
            const plateCollision& coll = collisions[i][j];
            uint32_t coll_count, coll_count_i, coll_count_j;
            float coll_ratio, coll_ratio_i, coll_ratio_j;

            ASSERT(i != coll.index, "when colliding: SRC == DEST!");

            // Collision causes friction. Apply it to both plates.
            plates[i]->applyFriction(coll.crust);
            plates[coll.index]->applyFriction(coll.crust);

            plates[i]->getCollisionInfo(coll.wx, coll.wy,
                                        &coll_count_i, &coll_ratio_i);
            plates[coll.index]->getCollisionInfo(coll.wx,
                                                 coll.wy, &coll_count_j, &coll_ratio_j);

            // Find the minimum count of collisions between two
            // continents on different plates.
            // It's minimum because large plate will get collisions
            // from all over whereas smaller plate will get just
            // a few. It's those few that matter between these two
            // plates, not what the big plate has with all the
            // other plates around it.
            coll_count = coll_count_i;
            coll_count -= (coll_count - coll_count_j) &
                          -(coll_count > coll_count_j);

            // Find maximum amount of collided surface area between
            // two continents on different plates.
            // Like earlier, it's the "experience" of the smaller
            // plate that matters here.
            coll_ratio = coll_ratio_i;
            coll_ratio += (coll_ratio_j - coll_ratio) *
                          (coll_ratio_j > coll_ratio);

            if ((coll_count > aggr_overlap_abs) |
                    (coll_ratio > aggr_overlap_rel))
            {
                float amount = plates[i]->aggregateCrust(
                                   plates[coll.index],
                                   coll.wx, coll.wy);

                // Calculate new direction and speed for the
                // merged plate system, that is, for the
                // receiving plate!
                plates[coll.index]->collide(*plates[i],
                                            coll.wx, coll.wy, amount);
            }
        }

        collisions[i].clear();
    }
}

// Remove empty plates from the system.
void lithosphere::removeEmptyPlates()
{
    for (uint32_t i = 0; i < num_plates; ++i)
    {
        if (num_plates == 1)
            puts("ONLY ONE PLATE LEFT!");
        else if (plate_indices_found[i] == 0)
        {
            delete plates[i];
            plates[i] = plates[num_plates - 1];
            plate_indices_found[i] = plate_indices_found[num_plates - 1];

            // Life is seldom as simple as seems at first.
            // Replace the moved plate's index in the index map
            // to match its current position in the array!
            for (uint32_t j = 0; j < _worldDimension.getArea(); ++j)
                if (imap[j] == num_plates - 1)
                    imap[j] = i;

            --num_plates;
            --i;
        }
    }
}

void lithosphere::update()
{
    try {
        _steps++;
        float totalVelocity = 0;
        float systemKineticEnergy = 0;

        for (uint32_t i = 0; i < num_plates; ++i)
        {
            totalVelocity += plates[i]->getVelocity();
            systemKineticEnergy += plates[i]->getMomentum();
        }

        if (systemKineticEnergy > peak_Ek) {
            peak_Ek = systemKineticEnergy;
        }

        // If there's no continental collisions during past iterations,
        // then interesting activity has ceased and we should restart.
        // Also if the simulation has been going on for too long already,
        // restart, because interesting stuff has most likely ended.
        if (totalVelocity < RESTART_SPEED_LIMIT ||
                systemKineticEnergy / peak_Ek < RESTART_ENERGY_RATIO ||
                last_coll_count > NO_COLLISION_TIME_LIMIT ||
                iter_count > RESTART_ITERATIONS)
        {
            restart();
            return;
        }

        const uint32_t map_area = _worldDimension.getArea();
        // Keep a copy of the previous index map
        prev_imap.copy(imap);

        // Realize accumulated external forces to each plate.
        for (uint32_t i = 0; i < num_plates; ++i)
        {
            plates[i]->resetSegments();

            if (erosion_period > 0 && iter_count % erosion_period == 0)
                plates[i]->erode(CONTINENTAL_BASE);

            plates[i]->move();
        }

        uint32_t oceanic_collisions = 0;
        uint32_t continental_collisions = 0;

        updateHeightAndPlateIndexMaps(map_area, oceanic_collisions, continental_collisions);

        // Update the counter of iterations since last continental collision.
        last_coll_count = (last_coll_count + 1) & -(continental_collisions == 0);

        for (uint32_t i = 0; i < num_plates; ++i)
        {
            for (uint32_t j = 0; j < subductions[i].size(); ++j)
            {
                const plateCollision& coll = subductions[i][j];

                ASSERT(i != coll.index, "when subducting: SRC == DEST!");

                // Do not apply friction to oceanic plates.
                // This is a very cheap way to emulate slab pull.
                // Just perform subduction and on our way we go!
                plates[i]->addCrustBySubduction(
                    coll.wx, coll.wy, coll.crust, iter_count,
                    plates[coll.index]->getVelX(),
                    plates[coll.index]->getVelY());
            }

            subductions[i].clear();
        }

        updateCollisions();

        fill(plate_indices_found.begin(), plate_indices_found.end(), 0);

        // Fill divergent boundaries with new crustal material, molten magma.
        for (uint32_t y = 0, i = 0; y < BOOL_REGENERATE_CRUST * _worldDimension.getHeight(); ++y) {
            for (uint32_t x = 0; x < _worldDimension.getWidth(); ++x, ++i) {
                if (imap[i] >= num_plates) {
                    // The owner of this new crust is that neighbour plate
                    // who was located at this point before plates moved.
                    imap[i] = prev_imap[i];

                    // If this is oceanic crust then add buoyancy to it.
                    // Magma that has just crystallized into oceanic crust
                    // is more buoyant than that which has had a lot of
                    // time to cool down and become more dense.
                    amap[i] = iter_count;
                    hmap[i] = OCEANIC_BASE * BUOYANCY_BONUS_X;

                    // This should probably not happen
                    if (imap[i] < num_plates) {
                        plates[imap[i]]->setCrust(x, y, OCEANIC_BASE,
                                                  iter_count);
                    }

                } else if (++plate_indices_found[imap[i]] && hmap[i] <= 0) {
                    puts("Occupied point has no land mass!");
                    exit(1);
                }
            }
        }

        removeEmptyPlates();

        //delete[] indexFound;

        // Add some "virginity buoyancy" to all pixels for a visual boost! :)
        for (uint32_t i = 0; i < (BUOYANCY_BONUS_X > 0) * map_area; ++i)
        {
            // Calculate the inverted age of this piece of crust.
            // Force result to be minimum between inv. age and
            // max buoyancy bonus age.
            uint32_t crust_age = iter_count - amap[i];
            crust_age = MAX_BUOYANCY_AGE - crust_age;
            crust_age &= -(crust_age <= MAX_BUOYANCY_AGE);

            hmap[i] += (hmap[i] < CONTINENTAL_BASE) * BUOYANCY_BONUS_X *
                       OCEANIC_BASE * crust_age * MULINV_MAX_BUOYANCY_AGE;
        }

        ++iter_count;
    } catch (const exception& e) {
        string msg = "Problem during update: ";
        msg = msg + e.what();
        cerr << msg << endl;
        throw runtime_error(msg.c_str());
    }
}

void lithosphere::restart()
{
    try {

        const uint32_t map_area = _worldDimension.getArea();

        cycle_count += max_cycles > 0; // No increment if running forever.
        if (cycle_count > max_cycles)
            return;

        // Update height map to include all recent changes.
        hmap.set_all(0);
        for (uint32_t i = 0; i < num_plates; ++i)
        {
            const uint32_t x0 = plates[i]->getLeftAsUint();
            const uint32_t y0 = plates[i]->getTopAsUint();
            const uint32_t x1 = x0 + plates[i]->getWidth();
            const uint32_t y1 = y0 + plates[i]->getHeight();

            const float*  this_map;
            const uint32_t* this_age;
            plates[i]->getMap(&this_map, &this_age);

            // Copy first part of plate onto world map.
            for (uint32_t y = y0, j = 0; y < y1; ++y)
            {
                for (uint32_t x = x0; x < x1; ++x, ++j)
                {
                    const uint32_t x_mod = _worldDimension.xMod(x);
                    const uint32_t y_mod = _worldDimension.yMod(y);
                    const float h0 = hmap[_worldDimension.indexOf(x_mod, y_mod)];
                    const float h1 = this_map[j];
                    const uint32_t a0 = amap[_worldDimension.indexOf(x_mod, y_mod)];
                    const uint32_t a1 =  this_age[j];

                    amap[_worldDimension.indexOf(x_mod, y_mod)] = (h0 *a0 +h1 *a1) /(h0 +h1);
                    hmap[_worldDimension.indexOf(x_mod, y_mod)] += this_map[j];
                }
            }
        }
        // Clear plate array
        clearPlates();

        // create new plates IFF there are cycles left to run!
        // However, if max cycle count is "ETERNITY", then 0 < 0 + 1 always.
        if (cycle_count < max_cycles + !max_cycles)
        {
            createPlates();

            // Restore the ages of plates' points of crust!
            for (uint32_t i = 0; i < num_plates; ++i)
            {
                const uint32_t x0 = plates[i]->getLeftAsUint();
                const uint32_t y0 = plates[i]->getTopAsUint();
                const uint32_t x1 = x0 + plates[i]->getWidth();
                const uint32_t y1 = y0 + plates[i]->getHeight();

                const float*  this_map;
                const uint32_t* this_age_const;
                uint32_t* this_age;

                plates[i]->getMap(&this_map, &this_age_const);
                this_age = (uint32_t *)this_age_const;

                for (uint32_t y = y0, j = 0; y < y1; ++y)
                {
                    for (uint32_t x = x0; x < x1; ++x, ++j)
                    {
                        const uint32_t x_mod = _worldDimension.xMod(x);
                        const uint32_t y_mod = _worldDimension.yMod(y);

                        this_age[j] = amap[_worldDimension.indexOf(x_mod, y_mod)];
                    }
                }
            }

            return;
        }

        // Add some "virginity buoyancy" to all pixels for a visual boost.
        for (uint32_t i = 0; i < (BUOYANCY_BONUS_X > 0) * map_area; ++i)
        {
            uint32_t crust_age = iter_count - amap[i];
            crust_age = MAX_BUOYANCY_AGE - crust_age;
            crust_age &= -(crust_age <= MAX_BUOYANCY_AGE);

            hmap[i] += (hmap[i] < CONTINENTAL_BASE) * BUOYANCY_BONUS_X *
                       OCEANIC_BASE * crust_age * MULINV_MAX_BUOYANCY_AGE;
        }
    } catch (const exception& e) {
        std::string msg = "Problem during restart: ";
        msg = msg + e.what();
        throw runtime_error(msg.c_str());
    }
}

uint32_t lithosphere::getWidth() const
{
    return _worldDimension.getWidth();
}

uint32_t lithosphere::getHeight() const
{
    return _worldDimension.getHeight();
}

uint32_t* lithosphere::getPlatesMap() const throw()
{
    return imap.raw_data();
}

const plate* lithosphere::getPlate(uint32_t index) const
{
    ASSERT(index < num_plates, "invalid plate index");
    return plates[index];
}
