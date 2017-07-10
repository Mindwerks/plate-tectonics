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

#ifndef LITHOSPHERE_HPP
#define LITHOSPHERE_HPP


#define NOMINMAX

#include <cstring> // For size_t.
#include <stdexcept>
#include <vector>
#include <memory>
#ifdef __MINGW32__ // this is to avoid a problem with the hypot function which is messed up by Python...
#undef __STRICT_ANSI__
#endif
#include <cmath>
#include "heightmap.hpp"
#include "simplerandom.hpp"

using namespace std;

#define CONTINENTAL_BASE 1.0f
#define OCEANIC_BASE     0.1f

class plate;

/**
* Wrapper for growing plate from a seed. Contains plate's dimensions.
*
* Used exclusively in plate creation phase.
*/
class plateArea
{
public:
    vector<uint32_t> border; ///< Plate's unprocessed border pixels.
    uint32_t btm; ///< Most bottom pixel of plate.
    uint32_t lft; ///< Most left pixel of plate.
    uint32_t rgt; ///< Most right pixel of plate.
    uint32_t top; ///< Most top pixel of plate.
    uint32_t wdt; ///< Width of area in pixels.
    uint32_t hgt; ///< Height of area in pixels.
};

/**
 * Lithosphere is the rigid outermost shell of a rocky planet.
 *
 * Lithosphere is divided into several rigid areas i.e. plates. In time passing
 * the topography of the planet evolves as the result of plate dynamics.
 * Lithosphere is the class responsible for creating and managing all the
 * plates. It updates heightmap to match the current setup of plates and thus
 * offers caller an easy access to system's topography.
 *
 */
class lithosphere
{
public:

    /**
     * Initialize system's height map i.e. topography.
     *
     * @param map_side_length Square height map's side's length in pixels.
     * @param sea_level Amount of surface area that becomes oceanic crust.
     * @param _erosion_period # of iterations between global erosion.
     * @param _folding_ratio Percent of overlapping crust that's folded.
     * @param aggr_ratio_abs # of overlapping points causing aggregation.
     * @param aggr_ratio_rel % of overlapping area causing aggregation.
     * @param num_cycles Number of times system will be restarted.
     * @exception	invalid_argument Exception is thrown if map side length
     *           	is not a power of two and greater than three.
     */
    lithosphere(long seed,
                uint32_t width, uint32_t height,
                float sea_level,
                uint32_t _erosion_period, float _folding_ratio,
                uint32_t aggr_ratio_abs, float aggr_ratio_rel,
                uint32_t num_cycles, uint32_t _max_plates);

    /**
     * Split the current topography into given number of (rigid) plates.
     *
     * Any previous set of plates is discarded.
     *
     * @param num_plates Number of areas the surface is divided into.
     */
    void createPlates();

    uint32_t getCycleCount() const {
        return cycle_count;
    }
    uint32_t getIterationCount() const {
        return iter_count;
    }
    const Dimension& getWorldDimension() const {
        return _worldDimension;
    }
    uint32_t getPlateCount() const; ///< Return number of plates.
    const uint32_t* getAgemap() const; ///< Return surface age map.
    float* getTopography(); ///< Return height map.
    uint32_t* getPlatesMap(); ///< Return a map of the plates owning eaach point
    void update(); ///< Simulate one step of plate tectonics.
    uint32_t getWidth() const;
    uint32_t getHeight() const;
    bool isFinished() const;
    const plate* getPlate(uint32_t index) const;

protected:
private:

    void createNoise(float* tmp, const Dimension& tmpDim, bool useSimplex = false);
    void createSlowNoise(float* tmp, const Dimension& tmpDim);
    uint32_t updateHeightAndPlateIndexMaps();
    void updateCollisions();
    void clearPlates();
    void growPlates();
    void removeEmptyPlates();
    void resolveJuxtapositions(const uint32_t i, const uint32_t ageMapValue, const float_t mapValue, 
                                  const Platec::vec2ui& p);

    /**
     * Container for collision details between two plates.
     *
     * In simulation there's usually 2-5 % collisions of the entire map
     * area. In a 512*512 map that means 5000-13000 collisions.
     *
     * When plate collisions are recorded and processed pair-by-pair, some
     * of the information is lost if more than two plates collide at the
     * same point (there will be no record of the two lower plates
     * colliding together, just that they both collided with the tallest
     * plate) ONLY IF ALL the collisions between ANY TWO plates of that
     * group always include a third, taller/higher  plate. This happens
     * most often when plates have long, sharp spikes i.e. in the
     * beginning.
     */
    class plateCollision
    {
    public:
        plateCollision(const uint32_t _index, const Platec::vec2ui& _point,const  float_t z)
        : index(_index), point(_point), crust(std::max(z,0.f)) {
        }
        const uint32_t index; ///< Index of the other plate involved in the event.
        const Platec::vec2ui point; ///< Coordinates of collision in world space.
        const float_t crust; ///< Amount of crust that will deform/subduct.
    };

    void restart(); //< Replace plates with a new population.

    HeightMap hmap; ///< Height map representing the topography of system.
    IndexMap imap; ///< Plate index map of the "owner" of each map point.
    IndexMap prev_imap; ///< Plate index map from the last update
    AgeMap amap; ///< Age map of the system's surface (topography).
    std::vector<std::unique_ptr<plate>> plates; ///< Array of plates that constitute the system.
    vector<plateArea> plate_areas;
    vector<uint32_t> plate_indices_found; ///< Used in update loop to remove plates

    uint32_t aggr_overlap_abs; ///< # of overlapping pixels -> aggregation.
    float  aggr_overlap_rel; ///< % of overlapping area -> aggregation.
    uint32_t cycle_count; ///< Number of times the system's been restarted.
    uint32_t erosion_period; ///< # of iterations between global erosion.
    float  folding_ratio; ///< Percent of overlapping crust that's folded.
    uint32_t iter_count; ///< Iteration count. Used to timestamp new crust.
    uint32_t max_cycles; ///< Max n:o of times the system'll be restarted.
    uint32_t max_plates; ///< Number of plates in the initial setting.
    uint32_t num_plates; ///< Number of plates in the current setting.

    vector<vector<plateCollision> > collisions;
    vector<vector<plateCollision> > subductions;

    float peak_Ek; ///< Max total kinetic energy in the system so far.
    uint32_t last_coll_count; ///< Iterations since last cont. collision.

    const Dimension _worldDimension;
    SimpleRandom _randsource;
    int _steps;
};


#endif
