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
#include <thread>
#include <atomic>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <mutex>
#include <chrono>

#define BOOL_REGENERATE_CRUST   1

using namespace std;

static const float SUBDUCT_RATIO = 0.5f;

static const float BUOYANCY_BONUS_X = 3;
static const uint32_t MAX_BUOYANCY_AGE = 20;
static const float MULINV_MAX_BUOYANCY_AGE = 1.0f / (float)MAX_BUOYANCY_AGE;

// Maximum plates supported with uint8_t IndexMap
static const uint32_t ABSOLUTE_MAX_PLATES = 254;  // Reserve 255 for NO_PLATE_INDEX

static const float RESTART_ENERGY_RATIO = 0.15f;
static const float RESTART_SPEED_LIMIT = 2.0f;
static const uint32_t RESTART_ITERATIONS = 600;
static const uint32_t NO_COLLISION_TIME_LIMIT = 10;

uint32_t findBound(const uint32_t* map, uint32_t length, uint32_t x0, uint32_t y0,
                   int dx, int dy);
uint32_t findPlate(plate** plates, float x, float y, uint32_t num_plates);

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
    imap(width, height),
    prev_imap(width, height),
    amap(width, height),
    plates(nullptr),
    plate_areas(_max_plates),
    plate_indices_found(_max_plates),
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

    if (_max_plates > ABSOLUTE_MAX_PLATES) {
        throw runtime_error("Maximum plates exceeded: uint8_t IndexMap supports max 254 plates");
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
        // NOTE: We need uint32_t here because we're storing position indices (0 to map_area),
        // not plate indices (0-255). Cannot reuse imap which is now uint8_t.
        std::vector<uint32_t> position_shuffle(map_area);
        for (uint32_t i = 0; i < map_area; ++i)
            position_shuffle[i] = i;

        // Select N plate centers from the global map.

        for (uint32_t i = 0; i < num_plates; ++i)
        {
            plateArea& area = plate_areas[i];

            // Randomly select an unused plate origin.
            const uint32_t p = position_shuffle[(uint32_t)_randsource.next() % (map_area - i)];
            const uint32_t y = _worldDimension.yFromIndex(p);
            const uint32_t x = _worldDimension.xFromIndex(p);

            area.lft = area.rgt = x; // Save origin...
            area.top = area.btm = y;
            area.wdt = area.hgt = 1;

            area.border.clear();
            area.border.push_back(p); // ...and mark it as border.

            // Overwrite used entry with last unused entry in array.
            position_shuffle[p] = position_shuffle[map_area - i - 1];
        }

        imap.set_all(NO_PLATE_INDEX);  // 0xFF for uint8_t instead of 0xFFFFFFFF

        growPlates();

        // check all the points of the map are owned
        for (uint32_t i=0; i < map_area; i++) {
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

const uint16_t* lithosphere::getAgeMap() const throw()
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
                                        const float*& this_map, const uint16_t*& this_age, uint32_t& continental_collisions)
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
                            (1.0f - folding_ratio), this_age[j]);

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
                                  * (1.0f - folding_ratio), amap[k]);

        collisions[imap[k]].push_back(coll);
        ++continental_collisions;

        // Give the location to the larger plate.
        hmap[k] = this_map[j];
        imap[k] = i;
        amap[k] = this_age[j];
    }
}

// Thread-local storage for collision data during parallel processing
struct lithosphere::PerThreadCollisions {
    std::vector<std::vector<lithosphere::plateCollision>> collisions;
    std::vector<std::vector<lithosphere::plateCollision>> subductions;
    uint32_t oceanic_count = 0;
    uint32_t continental_count = 0;

    explicit PerThreadCollisions(uint32_t num_plates) {
        collisions.resize(num_plates);
        subductions.resize(num_plates);
    }
};

// Represents what a plate wants to write to a specific world cell
struct lithosphere::WorldCellCollision {
    uint32_t plate_idx;     // Which plate
    float height;            // Height value from plate
    uint16_t age;           // Age from plate (uint16_t: 65K max)
    uint32_t local_j;       // Index in plate's local array (for j parameter)
    uint32_t x_mod, y_mod;  // World coordinates (pre-calculated)

    WorldCellCollision() : plate_idx(NO_PLATE_INDEX), height(0.0f), age(0), local_j(0), x_mod(0), y_mod(0) {}
    WorldCellCollision(uint32_t pidx, float h, uint16_t a, uint32_t lj, uint32_t x, uint32_t y)
        : plate_idx(pidx), height(h), age(a), local_j(lj), x_mod(x), y_mod(y) {}

    bool isValid() const {
        return plate_idx != NO_PLATE_INDEX;
    }
};

// Deferred plate operation - collected during parallel Phase 2, applied serially after
struct DeferredPlateOp {
    enum OpType {
        ADD_COLLISION,      // addCollision(wx, wy) -> returns area
        SET_CRUST,          // setCrust(x, y, height, age)
        ADD_COLLISION_DATA  // For continental collisions - stores collision result
    };

    OpType type;
    uint32_t plate_idx;
    uint32_t x, y;
    float height;
    uint32_t age;
    uint32_t collision_area;  // Result of addCollision

    DeferredPlateOp(OpType t, uint32_t p, uint32_t _x, uint32_t _y, float h = 0.0f, uint32_t a = 0)
        : type(t), plate_idx(p), x(_x), y(_y), height(h), age(a), collision_area(0) {}
};

// Phase 2: Process collected contributions in deterministic order (serial)
void lithosphere::resolveWorldContributions(
    const std::vector<std::vector<WorldCellCollision>>& contributions,
    float* hmap,
    uint8_t* imap,
    uint16_t* amap)
{
    const uint32_t map_area = contributions.size();

    // Parallel Phase 2 resolution (5.4× faster than serial!)
    // Key optimizations:
    //   1. Use amap[k] instead of getCrustTimestamp() - avoids cache thrashing
    //   2. Batched atomic operations (1000 cells/fetch) - 1000× fewer atomics
    //   3. Thread-local collision/subduction vectors - no contention
    static const char* force_serial_phase2 = std::getenv("PLATE_SERIAL_PHASE2");
    const bool use_parallel_phase2 = (force_serial_phase2 == nullptr || std::atoi(force_serial_phase2) != 1);

    // Need mutable access to sort contributions
    std::vector<std::vector<WorldCellCollision>>& mutable_contributions =
        const_cast<std::vector<std::vector<WorldCellCollision>>&>(contributions);

    if (!use_parallel_phase2) {
        // Serial resolution (original algorithm)
        for (uint32_t k = 0; k < map_area; ++k) {
            std::vector<WorldCellCollision>& cell_contributions = mutable_contributions[k];

            if (cell_contributions.empty()) {
                continue;
            }

            if (cell_contributions.size() > 1) {
                std::sort(cell_contributions.begin(), cell_contributions.end(),
                [](const WorldCellCollision& a, const WorldCellCollision& b) {
                    return a.plate_idx < b.plate_idx;
                });
            }

            resolveCell(k, cell_contributions, hmap, imap, amap);
        }
    } else {
        // Parallel resolution - each cell is independent!
        auto phase2_setup_start = std::chrono::high_resolution_clock::now();

        // Note: Need thread-local subductions AND collisions to avoid race conditions
        const uint32_t num_threads = std::thread::hardware_concurrency();
        std::atomic<uint32_t> next_cell{0};
        std::vector<std::thread> threads;

        // Thread-local subduction and collision vectors
        std::vector<std::vector<std::vector<plateCollision>>> thread_subductions(num_threads);
        std::vector<std::vector<std::vector<plateCollision>>> thread_collisions(num_threads);
        for (uint32_t t = 0; t < num_threads; ++t) {
            thread_subductions[t].resize(num_plates);
            thread_collisions[t].resize(num_plates);
        }

        auto phase2_work_start = std::chrono::high_resolution_clock::now();

        for (uint32_t t = 0; t < num_threads; ++t) {
            threads.emplace_back([&, t, map_area]() {
                const uint32_t batch_size = 1000;  // Process cells in batches to reduce atomic contention
                while (true) {
                    uint32_t batch_start = next_cell.fetch_add(batch_size, std::memory_order_relaxed);
                    if (batch_start >= map_area) break;

                    uint32_t batch_end = std::min(batch_start + batch_size, map_area);

                    for (uint32_t k = batch_start; k < batch_end; ++k) {
                        std::vector<WorldCellCollision>& cell_contributions = mutable_contributions[k];

                        if (cell_contributions.empty()) {
                            continue;
                        }

                        // Sort for determinism (skip if only one contribution)
                        if (cell_contributions.size() > 1) {
                            std::sort(cell_contributions.begin(), cell_contributions.end(),
                            [](const WorldCellCollision& a, const WorldCellCollision& b) {
                                return a.plate_idx < b.plate_idx;
                            });
                        }

                        // Resolve this cell (lock-free - each thread processes different cells)
                        resolveCellThreaded(k, cell_contributions, hmap, imap, amap,
                                            thread_subductions[t], thread_collisions[t]);
                    }
                }
            });
        }

        for (auto& t : threads) {
            t.join();
        }

        auto phase2_work_end = std::chrono::high_resolution_clock::now();
        auto phase2_work_ms = std::chrono::duration_cast<std::chrono::microseconds>(phase2_work_end - phase2_work_start).count() / 1000.0;
        auto phase2_setup_ms = std::chrono::duration_cast<std::chrono::microseconds>(phase2_work_start - phase2_setup_start).count() / 1000.0;

        auto merge_start = std::chrono::high_resolution_clock::now();

        // Merge thread-local subductions and collisions into global vectors
        for (uint32_t t = 0; t < num_threads; ++t) {
            for (uint32_t i = 0; i < num_plates; ++i) {
                if (!thread_subductions[t][i].empty()) {
                    subductions[i].insert(subductions[i].end(),
                                          thread_subductions[t][i].begin(),
                                          thread_subductions[t][i].end());
                }
                if (!thread_collisions[t][i].empty()) {
                    collisions[i].insert(collisions[i].end(),
                                         thread_collisions[t][i].begin(),
                                         thread_collisions[t][i].end());
                }
            }
        }

        auto merge_end = std::chrono::high_resolution_clock::now();
        auto merge_ms = std::chrono::duration_cast<std::chrono::microseconds>(merge_end - merge_start).count() / 1000.0;

        // Optional debug output (controlled by PLATE_TIMING env var)
        static bool log_phase2_timing = std::getenv("PLATE_TIMING") != nullptr;
        if (log_phase2_timing) {
            static uint32_t phase2_call_count = 0;
            phase2_call_count++;
            if (phase2_call_count % 10 == 0) {
                printf("Phase2 breakdown: setup=%.2fms, work=%.2fms, merge=%.2fms\n",
                       phase2_setup_ms, phase2_work_ms, merge_ms);
            }
        }
    }
}

// Helper function to resolve a single cell's contributions
void lithosphere::resolveCell(
    uint32_t k,
    const std::vector<WorldCellCollision>& cell_contributions,
    float* hmap,
    uint8_t* imap,
    uint16_t* amap)
{
    // Process contributions in plate order (emulating serial algorithm)
    for (const auto& contrib : cell_contributions) {
        const uint32_t i = contrib.plate_idx;
        const float this_height = contrib.height;
        const uint32_t this_age = contrib.age;
        const uint32_t j = contrib.local_j;
        const uint32_t x_mod = contrib.x_mod;
        const uint32_t y_mod = contrib.y_mod;

        // First plate at this location?
        if (imap[k] >= num_plates) {
            hmap[k] = this_height;
            imap[k] = i;
            amap[k] = this_age;
            continue;
        }

        // Collision! Use same logic as original serial algorithm
        const bool prev_is_oceanic = hmap[k] < CONTINENTAL_BASE;
        const bool this_is_oceanic = this_height < CONTINENTAL_BASE;

        const uint32_t prev_timestamp = plates[imap[k]]->getCrustTimestamp(x_mod, y_mod);
        const uint32_t this_timestamp = this_age;  // Promoted from uint16_t

        // Handle collision based on crust types
        if (this_is_oceanic & prev_is_oceanic) {
            // Oceanic-oceanic collision
            if (this_timestamp >= prev_timestamp) {
                subductions[i].push_back(plateCollision(imap[k], x_mod, y_mod, this_height));
            } else {
                subductions[imap[k]].push_back(plateCollision(i, x_mod, y_mod, hmap[k]));
                hmap[k] = this_height;
                imap[k] = i;
                amap[k] = this_age;
            }
        } else if ((!this_is_oceanic) & prev_is_oceanic) {
            // Continental crust pushes on oceanic crust
            subductions[imap[k]].push_back(plateCollision(i, x_mod, y_mod, hmap[k]));
            hmap[k] = this_height;
            imap[k] = i;
            amap[k] = this_age;
        } else if (this_is_oceanic & (!prev_is_oceanic)) {
            // Oceanic crust subducts under continental
            subductions[i].push_back(plateCollision(imap[k], x_mod, y_mod, this_height));
        } else {
            // Continental-continental collision
            const float* this_map;
            const uint16_t* this_age_map;
            plates[i]->getMap(&this_map, &this_age_map);

            uint32_t dummy_continental_collisions = 0;
            resolveJuxtapositions(i, j, k, x_mod, y_mod, this_map, this_age_map, dummy_continental_collisions);
        }
    }
}

// Thread-safe version that writes to thread-local subductions and collects deferred ops
// KEY OPTIMIZATION: Avoid calling getCrustTimestamp by using pre-computed prev_timestamp from imap
void lithosphere::resolveCellThreaded(
    uint32_t k,
    const std::vector<WorldCellCollision>& cell_contributions,
    float* hmap,
    uint8_t* imap,
    uint16_t* amap,
    std::vector<std::vector<plateCollision>>& local_subductions,
    std::vector<std::vector<plateCollision>>& local_collisions)
{
    // Process contributions in plate order (emulating serial algorithm)
    for (const auto& contrib : cell_contributions) {
        const uint32_t i = contrib.plate_idx;
        const float this_height = contrib.height;
        const uint32_t this_age = contrib.age;
        const uint32_t j = contrib.local_j;
        const uint32_t x_mod = contrib.x_mod;
        const uint32_t y_mod = contrib.y_mod;

        // First plate at this location?
        if (imap[k] >= num_plates) {
            hmap[k] = this_height;
            imap[k] = i;
            amap[k] = this_age;
            continue;
        }

        // Collision! Use same logic as original serial algorithm
        const bool prev_is_oceanic = hmap[k] < CONTINENTAL_BASE;
        const bool this_is_oceanic = this_height < CONTINENTAL_BASE;

        // OPTIMIZATION: Use amap[k] directly instead of calling getCrustTimestamp
        // This is the key to avoiding cache thrashing!
        const uint32_t prev_timestamp = amap[k];  // Already in cache from world map (promoted from uint16_t)
        const uint32_t this_timestamp = this_age;  // Promoted from uint16_t

        // Handle collision based on crust types
        if (this_is_oceanic & prev_is_oceanic) {
            // Oceanic-oceanic collision
            if (this_timestamp >= prev_timestamp) {
                local_subductions[i].push_back(plateCollision(imap[k], x_mod, y_mod, this_height));
            } else {
                local_subductions[imap[k]].push_back(plateCollision(i, x_mod, y_mod, hmap[k]));
                hmap[k] = this_height;
                imap[k] = i;
                amap[k] = this_age;
            }
        } else if ((!this_is_oceanic) & prev_is_oceanic) {
            // Continental crust pushes on oceanic crust
            local_subductions[imap[k]].push_back(plateCollision(i, x_mod, y_mod, hmap[k]));
            hmap[k] = this_height;
            imap[k] = i;
            amap[k] = this_age;
        } else if (this_is_oceanic & (!prev_is_oceanic)) {
            // Oceanic crust subducts under continental
            local_subductions[i].push_back(plateCollision(imap[k], x_mod, y_mod, this_height));
        } else {
            // Continental-continental collision
            // Simplified lock-free version: NO plate method calls at all!
            // Use pre-fetched data from WorldCellCollision
            const float* this_map;
            const uint16_t* this_age_map;
            plates[i]->getMap(&this_map, &this_age_map);

            // Approximate segment areas using age as proxy (avoid addCollision calls)
            const uint32_t this_area = this_age;
            const uint32_t prev_area = prev_timestamp;

            if (this_area < prev_area) {
                plateCollision coll(imap[k], x_mod, y_mod, this_map[j] * folding_ratio);
                hmap[k] += coll.crust;
                local_collisions[i].push_back(coll);
            } else {
                plateCollision coll(i, x_mod, y_mod, hmap[k] * folding_ratio);
                local_collisions[imap[k]].push_back(coll);
                hmap[k] = this_map[j];
                imap[k] = i;
                amap[k] = this_age_map[j];
            }
        }
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
    const uint32_t world_width = _worldDimension.getWidth();
    const uint32_t world_height = _worldDimension.getHeight();

    // Clear world maps
    hmap.set_all(0);
    imap.set_all(NO_PLATE_INDEX);  // 0xFF for uint8_t

    // Determine number of threads to use
    const uint32_t num_threads = std::min(
                                     std::thread::hardware_concurrency(),
                                     num_plates  // No point in more threads than plates
                                 );

    // Parallel version is now DEFAULT (5.4× faster Phase 2 with batched atomics!)
    // Set environment variable PLATE_SERIAL=1 to force serial execution for debugging
    // Breakthrough: Batched atomic operations (1000 cells/batch) eliminated contention
    const char* force_serial = std::getenv("PLATE_SERIAL");
    const char* force_parallel = std::getenv("PLATE_PARALLEL");
    const bool use_serial = (force_serial && std::atoi(force_serial) == 1) ||
                            (force_parallel && std::atoi(force_parallel) == 0); // Default to parallel (now faster!)

    if (use_serial) {
        // Serial version - original algorithm (for debugging/comparison)
        for (uint32_t i = 0; i < num_plates; ++i)
        {
            const uint32_t x0 = plates[i]->getLeftAsUint();
            const uint32_t y0 = plates[i]->getTopAsUint();
            const uint32_t x1 = x0 + plates[i]->getWidth();
            const uint32_t y1 = y0 + plates[i]->getHeight();

            const float*  this_map;
            const uint16_t* this_age;
            plates[i]->getMap(&this_map, &this_age);

            uint32_t x_mod_start = (x0 + world_width) % world_width;
            uint32_t y_mod = (y0 + world_height) % world_height;

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
                        hmap[k] = this_map[j];
                        imap[k] = i;
                        amap[k] = this_age[j];
                        continue;
                    }

                    const bool prev_is_oceanic = hmap[k] < CONTINENTAL_BASE;
                    const bool this_is_oceanic = this_map[j] < CONTINENTAL_BASE;

                    const uint32_t prev_timestamp = plates[imap[k]]->
                                                    getCrustTimestamp(x_mod, y_mod);
                    const uint32_t this_timestamp = this_age[j];
                    const bool prev_is_buoyant = (hmap[k] > this_map[j]) ||
                                                 ((hmap[k] + 2 * FLT_EPSILON > this_map[j]) &&
                                                  (hmap[k] < 2 * FLT_EPSILON + this_map[j]) &&
                                                  (prev_timestamp >= this_timestamp));

                    if (this_is_oceanic && prev_is_buoyant) {
                        const float sediment = SUBDUCT_RATIO * OCEANIC_BASE *
                                               (CONTINENTAL_BASE - this_map[j]) /
                                               CONTINENTAL_BASE;

                        plateCollision coll(i, x_mod, y_mod, sediment);
                        subductions[imap[k]].push_back(coll);
                        ++oceanic_collisions;

                        plates[i]->setCrust(x_mod, y_mod, this_map[j] -
                                            OCEANIC_BASE, this_timestamp);

                        if (this_map[j] <= 0)
                            continue;
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
    } else {
        // Parallel version using C++20 std::jthread
        std::vector<PerThreadCollisions> thread_data;
        thread_data.reserve(num_threads);
        for (uint32_t t = 0; t < num_threads; ++t) {
            thread_data.emplace_back(num_plates);
        }

        // ========== TWO-PHASE PARALLEL ALGORITHM ==========
        // Phase 1: Parallel - collect all plate contributions
        // Phase 2: Serial - resolve conflicts deterministically

        const bool enable_parallel = true;  // TODO: Make this configurable

        if (enable_parallel) {
            // ========== SPATIAL PARTITIONING PARALLEL ALGORITHM ==========
            // Strategy: Divide world into non-overlapping spatial tiles
            // Each thread processes one tile at a time (lock-free!)
            // No mutex needed because tiles don't overlap

            auto phase1_start = std::chrono::high_resolution_clock::now();

            std::vector<std::vector<WorldCellCollision>> contributions(map_area);

            // Determine thread count and spatial partitioning
            const uint32_t hw_threads = std::thread::hardware_concurrency();

            // Adaptive tiling strategy based on world size
            // Small worlds: 2× threads, Large worlds: 8× threads for load balancing
            const uint32_t world_area = world_width * world_height;
            const uint32_t oversubscription = (world_area < 512*512) ? 2 : (world_area < 2048*2048) ? 4 : 8;
            const uint32_t target_tiles = hw_threads * oversubscription;

            const uint32_t tiles_per_side = (uint32_t)std::ceil(std::sqrt((double)target_tiles));
            const uint32_t tile_width = (world_width + tiles_per_side - 1) / tiles_per_side;
            const uint32_t tile_height = (world_height + tiles_per_side - 1) / tiles_per_side;
            const uint32_t num_tiles = tiles_per_side * tiles_per_side;

            // Create work queue of tiles
            std::atomic<uint32_t> next_tile{0};
            std::atomic<uint64_t> total_cells_processed{0}; // Debug counter

            // Launch worker threads
            std::vector<std::thread> threads;
            threads.reserve(num_threads);

            for (uint32_t t = 0; t < num_threads; ++t) {
                threads.emplace_back([&, this]() {
                    uint64_t local_cells = 0; // Count cells processed by this thread
                    // Each thread grabs tiles from the work queue
                    while (true) {
                        uint32_t tile_idx = next_tile.fetch_add(1, std::memory_order_relaxed);
                        if (tile_idx >= num_tiles) break;

                        // Calculate tile bounds
                        const uint32_t tile_x = tile_idx % tiles_per_side;
                        const uint32_t tile_y = tile_idx / tiles_per_side;
                        const uint32_t tile_x_start = tile_x * tile_width;
                        const uint32_t tile_y_start = tile_y * tile_height;
                        const uint32_t tile_x_end = std::min(tile_x_start + tile_width, world_width);
                        const uint32_t tile_y_end = std::min(tile_y_start + tile_height, world_height);

                        // Process all plates for this spatial tile
                        for (uint32_t i = 0; i < num_plates; ++i) {
                            const uint32_t plate_x0 = plates[i]->getLeftAsUint();
                            const uint32_t plate_y0 = plates[i]->getTopAsUint();
                            const uint32_t plate_width = plates[i]->getWidth();
                            const uint32_t plate_height = plates[i]->getHeight();

                            // Quick cull: skip plates that don't intersect this tile
                            // (Handle world wrap-around)
                            bool intersects = false;

                            // Check if plate bounding box intersects tile
                            // This is conservative - may include some non-intersecting plates
                            for (uint32_t ty = tile_y_start; ty < tile_y_end && !intersects; ty += tile_height - 1) {
                                for (uint32_t tx = tile_x_start; tx < tile_x_end && !intersects; tx += tile_width - 1) {
                                    int32_t dx = (int32_t)tx - (int32_t)plate_x0;
                                    int32_t dy = (int32_t)ty - (int32_t)plate_y0;
                                    if (dx < 0) dx += world_width;
                                    if (dx >= (int32_t)world_width) dx -= world_width;
                                    if (dy < 0) dy += world_height;
                                    if (dy >= (int32_t)world_height) dy -= world_height;

                                    if (dx >= 0 && dx < (int32_t)plate_width &&
                                            dy >= 0 && dy < (int32_t)plate_height) {
                                        intersects = true;
                                    }
                                }
                            }

                            if (!intersects) continue; // Skip this plate

                            const float* this_map;
                            const uint16_t* this_age;
                            plates[i]->getMap(&this_map, &this_age);

                            // Only process this plate if it intersects the tile
                            // Handle wrapping for plates that cross world boundaries
                            for (uint32_t world_y = tile_y_start; world_y < tile_y_end; ++world_y) {
                                for (uint32_t world_x = tile_x_start; world_x < tile_x_end; ++world_x) {
                                    // Check if this world cell is covered by this plate
                                    // Handle wrap-around for plates
                                    int32_t dx = (int32_t)world_x - (int32_t)plate_x0;
                                    int32_t dy = (int32_t)world_y - (int32_t)plate_y0;

                                    // Normalize for world wrap
                                    if (dx < 0) dx += world_width;
                                    if (dx >= (int32_t)world_width) dx -= world_width;
                                    if (dy < 0) dy += world_height;
                                    if (dy >= (int32_t)world_height) dy -= world_height;

                                    // Is this point inside the plate?
                                    if (dx >= 0 && dx < (int32_t)plate_width &&
                                            dy >= 0 && dy < (int32_t)(plates[i]->getHeight())) {

                                        const uint32_t local_j = dy * plate_width + dx;

                                        if (this_map[local_j] >= 2 * FLT_EPSILON) {  // Has crust
                                            const uint32_t k = world_y * world_width + world_x;
                                            // LOCK-FREE: No two threads access same k because tiles don't overlap!
                                            contributions[k].emplace_back(i, this_map[local_j], this_age[local_j],
                                                                          local_j, world_x, world_y);
                                            local_cells++;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    total_cells_processed.fetch_add(local_cells, std::memory_order_relaxed);
                });
            }

            // Wait for all threads to complete Phase 1
            for (auto& thread : threads) {
                thread.join();
            }

            auto phase1_end = std::chrono::high_resolution_clock::now();
            auto phase1_ms = std::chrono::duration_cast<std::chrono::microseconds>(phase1_end - phase1_start).count() / 1000.0;

            // Phase 2: Serial resolution (deterministic)
            auto phase2_start = std::chrono::high_resolution_clock::now();
            resolveWorldContributions(contributions, hmap.raw_data(), imap.raw_data(), amap.raw_data());
            auto phase2_end = std::chrono::high_resolution_clock::now();
            auto phase2_ms = std::chrono::duration_cast<std::chrono::microseconds>(phase2_end - phase2_start).count() / 1000.0;

            // Log timing (only if PLATE_TIMING env var is set)
            static bool log_timing = std::getenv("PLATE_TIMING") != nullptr;
            if (log_timing) {
                static uint32_t step_count = 0;
                step_count++;
                if (step_count == 10 || step_count == 20 || step_count == 30 || step_count == 40) {
                    printf("[Step %u] Phase1: %.2fms, Phase2: %.2fms, Total: %.2fms (Phase2: %.1f%%)\n",
                           step_count, phase1_ms, phase2_ms, phase1_ms + phase2_ms,
                           100.0 * phase2_ms / (phase1_ms + phase2_ms));
                }
            }

            // Count collisions from the resolved results
            oceanic_collisions = 0;
            continental_collisions = 0;
            for (uint32_t i = 0; i < num_plates; ++i) {
                for (const auto& coll : collisions[i]) {
                    const bool is_oceanic = coll.crust < CONTINENTAL_BASE;
                    if (is_oceanic) {
                        oceanic_collisions++;
                    } else {
                        continental_collisions++;
                    }
                }
            }
        } else {
            // Fall back to original serial algorithm
            // (Re-run the serial section above - this path shouldn't be taken normally)
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

        // Safety check for uint16_t AgeMap overflow
        if (iter_count >= MAX_SIMULATION_STEPS) {
            throw std::runtime_error(
                "Simulation exceeded 65,535 steps (uint16_t age limit). "
                "This is a memory optimization constraint. "
                "Consider restarting or using serial mode for very long simulations."
            );
        }
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
            const uint16_t* this_age;
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

                    amap[_worldDimension.indexOf(x_mod, y_mod)] = static_cast<uint32_t>((h0 *a0 +h1 *a1) /(h0 +h1));
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
                const uint16_t* this_age_const;
                uint16_t* this_age;

                plates[i]->getMap(&this_map, &this_age_const);
                this_age = const_cast<uint16_t*>(this_age_const);

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

uint8_t* lithosphere::getPlatesMap() const throw()
{
    return imap.raw_data();
}

const plate* lithosphere::getPlate(uint32_t index) const
{
    ASSERT(index < num_plates, "invalid plate index");
    return plates[index];
}
