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

#define _USE_MATH_DEFINES // Winblow$.
#include <cfloat>         // FT_EPSILON
#ifdef __MINGW32__ // this is to avoid a problem with the hypot function which is messed up by
                   // Python...
    #undef __STRICT_ANSI__
#endif
#include "simd_utils.hpp"
#include <assert.h>
#include <cmath>     // sin, cos
#include <cstdlib>   // rand
#include <stdexcept> // std::invalid_argument
#include <vector>

#include "heightmap.hpp"
#include "plate.hpp"
#include "plate_functions.hpp"
#include "rectangle.hpp"
#include "utils.hpp"

using namespace std;

plate::plate(long seed, float* m, uint32_t w, uint32_t h, uint32_t _x, uint32_t _y,
             uint32_t plate_age, WorldDimension worldDimension)
    : _worldDimension(worldDimension), _randsource(seed), map(m, w, h), age_map(w, h),
      _bounds(nullptr), _mass(MassBuilder(m, Dimension(w, h)).build()),
      _movement(_randsource, worldDimension), _segments(nullptr), _mySegmentCreator(nullptr) {
    const uint32_t plate_area = w * h;

    _bounds = new Bounds(worldDimension, FloatPoint(static_cast<float>(_x), static_cast<float>(_y)),
                         Dimension(w, h));

    uint32_t k;
    for (uint32_t y = k = 0; y < _bounds->height(); ++y) {
        for (uint32_t x = 0; x < _bounds->width(); ++x, ++k) {
            // Set the age of ALL points in this plate to same
            // value. The right thing to do would be to simulate
            // the generation of new oceanic crust as if the plate
            // had been moving to its current direction until all
            // plate's (oceanic) crust receive an age.
            age_map.set(x, y, plate_age & -(m[k] > 0));
        }
    }
    Segments* segments = new Segments(plate_area);
    _segments = segments;
    _mySegmentCreator = new MySegmentCreator(*_bounds, _segments, map, _worldDimension);
    segments->setSegmentCreator(_mySegmentCreator);
    segments->setBounds(_bounds);
}

plate::~plate() {
    delete _mySegmentCreator;
    delete _segments;
    delete _bounds;
}

uint32_t plate::addCollision(uint32_t wx, uint32_t wy) {
    ISegmentData& seg = getContinentAt(wx, wy);
    seg.incCollCount();
    return seg.area();
}

void plate::addCrustByCollision(uint32_t x, uint32_t y, float z, uint32_t time,
                                ContinentId activeContinent) {
    // Add crust. Extend plate if necessary.
    setCrust(x, y, getCrust(x, y) + z, time);

    uint32_t index = _bounds->getValidMapIndex(&x, &y);
    _segments->setId(index, activeContinent);

    ISegmentData& data = (*_segments)[activeContinent];
    data.incArea();
    data.enlarge_to_contain(x, y);
}

void plate::addCrustBySubduction(uint32_t x, uint32_t y, float z, uint32_t t, float dx, float dy) {
    // TODO: Create an array of coordinate changes that would create
    //       a circle around current point. Array is static and it is
    //       initialized at the first call to this function.
    //       After all points of the circle are checked around subduction
    //       point the one with most land mass around it will be chosen as
    //       "most in land" point and subducting crust is added there.
    //       However to achieve a little more "natural" look normal
    //       distributed randomness is added around the "center" point.
    //       Benefits:
    //           NEVER adds crust outside plate.
    //           ALWAYS goes inland as much as possible
    //       Drawbacks:
    //           Additional logic required
    //           Might place crust on other continent on same plate!
    _bounds->getValidMapIndex(&x, &y);

    // Take vector difference only between plates that move more or less
    // to same direction. This makes subduction direction behave better.
    //
    // Use of "this" pointer is not necessary, but it make code clearer.
    // Cursed be those who use "m_" prefix in member names! >(
    float dot = _movement.dot(dx, dy);
    dx -= _movement.velocityOnX(dot > 0);
    dy -= _movement.velocityOnY(dot > 0);

    float offset = static_cast<float>(_randsource.next_double());
    float offset_sign = static_cast<float>(2 * static_cast<int>(_randsource.next() % 2) - 1);
    offset *= offset * offset * offset_sign;
    float offset2 = static_cast<float>(_randsource.next_double());
    float offset_sign2 = static_cast<float>(2 * static_cast<int>(_randsource.next() % 2) - 1);
    offset2 *= offset2 * offset2 * offset_sign2;
    dx = 10 * dx + 3 * offset;
    dy = 10 * dy + 3 * offset2;

    float fx = x + dx;
    float fy = y + dy;
    uint32_t index = 0;

    if (_bounds->isInLimits(fx, fy)) {
        index = _bounds->index(static_cast<uint32_t>(fx), static_cast<uint32_t>(fy));
        if (map[index] > 0) {
            t = (map[index] * age_map[index] + z * t) / (map[index] + z);
            age_map[index] =
                static_cast<uint32_t>(static_cast<float>(t) * static_cast<float>(z > 0));

            map[index] += z;
            _mass.incMass(z);
        }
    }
}

float plate::aggregateCrust(plate* p, uint32_t wx, uint32_t wy) {
    uint32_t lx = wx;
    uint32_t ly = wy;
    const uint32_t index = _bounds->getValidMapIndex(&lx, &ly);

    const ContinentId seg_id = _segments->id(index);

    // This check forces the caller to do things in proper order!
    //
    // Usually continents collide at several locations simultaneously.
    // Thus if this segment that is being merged now is removed from
    // segmentation bookkeeping, then the next point of collision that is
    // processed during the same iteration step would cause the test
    // below to be true and system would experience a premature abort.
    //
    // Therefore, segmentation bookkeeping is left intact. It doesn't
    // cause significant problems because all crust is cleared and empty
    // points are not processed at all. (Test on (seg_id >= seg_data.size()) removed)

    // One continent may have many points of collision. If one of them
    // causes continent to aggregate then all successive collisions and
    // attempts of aggregation would necessarily change nothing at all,
    // because the continent was removed from this plate earlier!
    if ((*_segments)[seg_id].isEmpty()) {
        return 0; // Do not process empty continents.
    }

    ContinentId activeContinent = p->selectCollisionSegment(wx, wy);

    // Wrap coordinates around world edges to safeguard subtractions.
    wx += _worldDimension.getWidth();
    wy += _worldDimension.getHeight();

    // Aggregating segment [%u, %u]x[%u, %u] vs. [%u, %u]@[%u, %u]\n",
    //      seg_data[seg_id].x0, seg_data[seg_id].y0,
    //      seg_data[seg_id].x1, seg_data[seg_id].y1,
    //      _dimension.getWidth(), _dimension.getHeight(), lx, ly);

    float old_mass = _mass.getMass();

    // Add all of the collided continent's crust to destination plate.
    for (uint32_t y = (*_segments)[seg_id].getTop(); y <= (*_segments)[seg_id].getBottom(); ++y) {
        for (uint32_t x = (*_segments)[seg_id].getLeft(); x <= (*_segments)[seg_id].getRight();
             ++x) {
            const uint32_t i = y * _bounds->width() + x;
            if ((_segments->id(i) == seg_id) && (map[i] > 0)) {
                p->addCrustByCollision(wx + x - lx, wy + y - ly, map[i], age_map[i],
                                       activeContinent);

                _mass.incMass(-1.0f * map[i]);
                map[i] = 0.0f;
            }
        }
    }

    (*_segments)[seg_id].markNonExistent(); // Mark segment as non-existent
    return old_mass - _mass.getMass();
}

void plate::applyFriction(float deformed_mass) {
    // Remove the energy that deformation consumed from plate's kinetic
    // energy: F - dF = ma - dF => a = dF/m.
    if (!_mass.null()) {
        _movement.applyFriction(deformed_mass, _mass.getMass());
    }
}

void plate::collide(plate& p, uint32_t wx, uint32_t wy, float coll_mass) {
    if (!_mass.null() && coll_mass > 0) {
        _movement.collide(_mass, p, wx, wy, coll_mass);
    }
}

void plate::calculateCrust(uint32_t x, uint32_t y, uint32_t index, float& w_crust, float& e_crust,
                           float& n_crust, float& s_crust, uint32_t& w, uint32_t& e, uint32_t& n,
                           uint32_t& s) {
    ::calculateCrust(x, y, index, w_crust, e_crust, n_crust, s_crust, w, e, n, s, _worldDimension,
                     map, _bounds->width(), _bounds->height());
}

void plate::findRiverSources(float lower_bound, vector<uint32_t>* sources) {
    const uint32_t bounds_height = _bounds->height();
    const uint32_t bounds_width = _bounds->width();

    // Find all tops.
    for (uint32_t y = 0; y < bounds_height; ++y) {
        const uint32_t y_width = y * bounds_width;
        for (uint32_t x = 0; x < bounds_width; ++x) {
            const uint32_t index = y_width + x;

            if (map[index] < lower_bound) {
                continue;
            }

            float w_crust;
            float e_crust;
            float n_crust;
            float s_crust;
            uint32_t w;
            uint32_t e;
            uint32_t n;
            uint32_t s;
            calculateCrust(x, y, index, w_crust, e_crust, n_crust, s_crust, w, e, n, s);

            // This location is either at the edge of the plate or it is not the
            // tallest of its neightbours. Don't start a river from here.
            if (w_crust * e_crust * n_crust * s_crust == 0) {
                continue;
            }

            sources->push_back(index);
        }
    }
}

void plate::flowRivers(float lower_bound, vector<uint32_t>* sources, HeightMap& tmp) {
    const uint32_t bounds_area = _bounds->area();
    vector<uint32_t> sinks_data;
    vector<uint32_t>* sinks = &sinks_data;

    static vector<bool> s_flowDone;
    if (s_flowDone.size() < bounds_area) {
        s_flowDone.resize(bounds_area);
    }
    fill(s_flowDone.begin(), s_flowDone.begin() + bounds_area, false);

    // From each top, start flowing water along the steepest slope.
    while (!sources->empty()) {
        while (!sources->empty()) {
            const uint32_t index = sources->back();
            const uint32_t y = index / _bounds->width();
            const uint32_t x = index - y * _bounds->width();

            sources->pop_back();

            if (map[index] < lower_bound) {
                continue;
            }

            float w_crust;
            float e_crust;
            float n_crust;
            float s_crust;
            uint32_t w;
            uint32_t e;
            uint32_t n;
            uint32_t s;
            calculateCrust(x, y, index, w_crust, e_crust, n_crust, s_crust, w, e, n, s);

            // If this is the lowest part of its neighbourhood, stop.
            if (w_crust + e_crust + n_crust + s_crust == 0) {
                continue;
            }

            w_crust += (w_crust == 0) * map[index];
            e_crust += (e_crust == 0) * map[index];
            n_crust += (n_crust == 0) * map[index];
            s_crust += (s_crust == 0) * map[index];

            // Find lowest neighbour.
            float lowest_crust = w_crust;
            uint32_t dest = index - 1;

            if (e_crust < lowest_crust) {
                lowest_crust = e_crust;
                dest = index + 1;
            }

            if (n_crust < lowest_crust) {
                lowest_crust = n_crust;
                dest = index - _bounds->width();
            }

            if (s_crust < lowest_crust) {
                lowest_crust = s_crust;
                dest = index + _bounds->width();
            }

            // if it's not handled yet, add it as new sink.
            if (dest < _bounds->area() && !s_flowDone[dest]) {
                sinks->push_back(dest);
                s_flowDone[dest] = true;
            }

            // Erode this location with the water flow.
            tmp[index] -= (tmp[index] - lower_bound) * 0.2f;
        }

        vector<uint32_t>* v_tmp = sources;
        sources = sinks;
        sinks = v_tmp;
        sinks->clear();
    }
}

void plate::erode(float lower_bound) {
    vector<uint32_t> sources_data;
    vector<uint32_t>* sources = &sources_data;

    HeightMap tmpHm(map);
    findRiverSources(lower_bound, sources);
    flowRivers(lower_bound, sources, tmpHm);

    // Add random noise (10 %) to heightmap.
    for (uint32_t i = 0; i < _bounds->area(); ++i) {
        float alpha = 0.2f * static_cast<float>(_randsource.next_double());
        tmpHm[i] += 0.1f * tmpHm[i] - alpha * tmpHm[i];
    }

    // Clamp to zero to prevent floating point errors from accumulating
    // and causing negative mass values (Issue #30)
    simd::clamp_non_negative(tmpHm.raw_data(), _bounds->area());

    map = tmpHm;
    tmpHm.set_all(0.0f);
    MassBuilder massBuilder;

    for (uint32_t y = 0; y < _bounds->height(); ++y) {
        for (uint32_t x = 0; x < _bounds->width(); ++x) {
            const uint32_t index = y * _bounds->width() + x;
            massBuilder.addPoint(x, y, map[index]);
            tmpHm[index] += map[index]; // Careful not to overwrite earlier amounts.

            if (map[index] < lower_bound) {
                continue;
            }

            float w_crust;
            float e_crust;
            float n_crust;
            float s_crust;
            uint32_t w;
            uint32_t e;
            uint32_t n;
            uint32_t s;
            calculateCrust(x, y, index, w_crust, e_crust, n_crust, s_crust, w, e, n, s);

            // This location has no neighbours (ARTIFACT!) or it is the lowest
            // part of its area. In either case the work here is done.
            if (w_crust + e_crust + n_crust + s_crust == 0) {
                continue;
            }

            // The steeper the slope, the more water flows along it.
            // The more downhill (sources), the more water flows to here.
            // 1+1+10 = 12, avg = 4, stdev = sqrt((3*3+3*3+6*6)/3) = 4.2, var = 18,
            //  1*1+1*1+10*10 = 102, 102/4.2=24
            // 1+4+7 = 12, avg = 4, stdev = sqrt((3*3+0*0+3*3)/3) = 2.4, var = 6,
            //  1*1+4*4+7*7 = 66, 66/2.4 = 27
            // 4+4+4 = 12, avg = 4, stdev = sqrt((0*0+0*0+0*0)/3) = 0, var = 0,
            //  4*4+4*4+4*4 = 48, 48/0 = inf -> 48
            // If there's a source slope of height X then it will always cause
            // water erosion of amount Y. Then again from one spot only so much
            // water can flow.
            // Thus, the calculated non-linear flow value for this location is
            // multiplied by the "water erosion" constant.
            // The result is max(result, 1.0). New height of this location could
            // be e.g. h_lowest + (1 - 1 / result) * (h_0 - h_lowest).

            // Calculate the difference in height between this point and its
            // nbours that are lower than this point.
            float w_diff = map[index] - w_crust;
            float e_diff = map[index] - e_crust;
            float n_diff = map[index] - n_crust;
            float s_diff = map[index] - s_crust;

            float min_diff = w_diff;
            min_diff -= (min_diff - e_diff) * (e_diff < min_diff);
            min_diff -= (min_diff - n_diff) * (n_diff < min_diff);
            min_diff -= (min_diff - s_diff) * (s_diff < min_diff);

            // Calculate the sum of difference between lower neighbours and
            // the TALLEST lower neighbour.
            float diff_sum =
                (w_diff - min_diff) * (w_crust > 0) + (e_diff - min_diff) * (e_crust > 0) +
                (n_diff - min_diff) * (n_crust > 0) + (s_diff - min_diff) * (s_crust > 0);

            // Erosion difference sum is negative!
            ASSERT(diff_sum >= 0, "Difference sum must be positive");

            if (diff_sum < min_diff) {
                // There's NOT enough room in neighbours to contain all the
                // crust from this peak so that it would be as tall as its
                // tallest lower neighbour. Thus first step is make ALL
                // lower neighbours and this point equally tall.
                tmpHm[w] += (w_diff - min_diff) * (w_crust > 0);
                tmpHm[e] += (e_diff - min_diff) * (e_crust > 0);
                tmpHm[n] += (n_diff - min_diff) * (n_crust > 0);
                tmpHm[s] += (s_diff - min_diff) * (s_crust > 0);
                tmpHm[index] -= min_diff;

                min_diff -= diff_sum;

                // Spread the remaining crust equally among all lower nbours.
                min_diff /= 1 + (w_crust > 0) + (e_crust > 0) + (n_crust > 0) + (s_crust > 0);

                tmpHm[w] += min_diff * (w_crust > 0);
                tmpHm[e] += min_diff * (e_crust > 0);
                tmpHm[n] += min_diff * (n_crust > 0);
                tmpHm[s] += min_diff * (s_crust > 0);
                tmpHm[index] += min_diff;
            } else {
                float unit = min_diff / diff_sum;

                // Remove all crust from this location making it as tall as
                // its tallest lower neighbour.
                tmpHm[index] -= min_diff;

                // Spread all removed crust among all other lower neighbours.
                tmpHm[w] += unit * (w_diff - min_diff) * (w_crust > 0);
                tmpHm[e] += unit * (e_diff - min_diff) * (e_crust > 0);
                tmpHm[n] += unit * (n_diff - min_diff) * (n_crust > 0);
                tmpHm[s] += unit * (s_diff - min_diff) * (s_crust > 0);
            }
        }
    }

    // Clamp all heightmap values to prevent negative mass from floating point errors
    // This is a safety measure for Issue #30
    simd::clamp_non_negative(tmpHm.raw_data(), _bounds->area());

    map = tmpHm;
    _mass = massBuilder.build();
}

void plate::getCollisionInfo(uint32_t wx, uint32_t wy, uint32_t* count, float* ratio) const {
    const ISegmentData& seg = getContinentAt(wx, wy);

    *count = seg.collCount();
    *ratio = (float)seg.collCount() / (float)(1 + seg.area()); // +1 avoids DIV with zero.
}

uint32_t plate::getContinentArea(uint32_t wx, uint32_t wy) const {
    const uint32_t index = _bounds->getValidMapIndex(&wx, &wy);
    ASSERT(_segments->id(index) < _segments->size(), "Segment index invalid");
    return (*_segments)[_segments->id(index)].area();
}

float plate::getCrust(uint32_t x, uint32_t y) const {
    const uint32_t index = _bounds->getMapIndex(&x, &y);
    return index != BAD_INDEX ? map[index] : 0;
}

uint32_t plate::getCrustTimestamp(uint32_t x, uint32_t y) const {
    const uint32_t index = _bounds->getMapIndex(&x, &y);
    return index != BAD_INDEX ? age_map[index] : 0;
}

void plate::getMap(const float** c, const uint32_t** t) const {
    if (c) {
        *c = map.raw_data();
    }
    if (t) {
        *t = age_map.raw_data();
    }
}

void plate::move() {
    _movement.move();

    // Location modulations into range [0..world width/height[ are a have to!
    // If left undone SOMETHING WILL BREAK DOWN SOMEWHERE in the code!

    _bounds->shift(_movement.velocityOnX(), _movement.velocityOnY());
}

void plate::resetSegments() {
    ASSERT(_bounds->area() == _segments->area(), "Segments doesn't have the expected area");
    _segments->reset();
}

void plate::setCrust(uint32_t x, uint32_t y, float z, uint32_t t) {
    if (z < 0) { // Do not accept negative values.
        z = 0;
    }

    uint32_t _x = x;
    uint32_t _y = y;
    uint32_t index = _bounds->getMapIndex(&_x, &_y);

    if (index == BAD_INDEX) {
        // Extending plate for nothing!
        ASSERT(z > 0, "Height value must be non-zero");

        const uint32_t ilft = _bounds->leftAsUint();
        const uint32_t itop = _bounds->topAsUint();
        const uint32_t irgt = _bounds->rightAsUintNonInclusive();
        const uint32_t ibtm = _bounds->bottomAsUintNonInclusive();

        _worldDimension.normalize(x, y);

        // Calculate distance of new point from plate edges.
        const uint32_t _lft = ilft - x;
        const uint32_t _rgt = (_worldDimension.getWidth() & -(x < ilft)) + x - irgt;
        const uint32_t _top = itop - y;
        const uint32_t _btm = (_worldDimension.getHeight() & -(y < itop)) + y - ibtm;

        // Set larger of horizontal/vertical distance to zero.
        // A valid distance is NEVER larger than world's side's length!
        uint32_t d_lft = _lft & -(_lft < _rgt) & -(_lft < _worldDimension.getWidth());
        uint32_t d_rgt = _rgt & -(_rgt <= _lft) & -(_rgt < _worldDimension.getWidth());
        uint32_t d_top = _top & -(_top < _btm) & -(_top < _worldDimension.getHeight());
        uint32_t d_btm = _btm & -(_btm <= _top) & -(_btm < _worldDimension.getHeight());

        // Scale all changes to multiple of 8.
        d_lft = ((d_lft > 0) + (d_lft >> 3)) << 3;
        d_rgt = ((d_rgt > 0) + (d_rgt >> 3)) << 3;
        d_top = ((d_top > 0) + (d_top >> 3)) << 3;
        d_btm = ((d_btm > 0) + (d_btm >> 3)) << 3;

        // Make sure plate doesn't grow bigger than the system it's in!
        if (_bounds->width() + d_lft + d_rgt > _worldDimension.getWidth()) {
            d_lft = 0;
            d_rgt = _worldDimension.getWidth() - _bounds->width();
        }

        if (_bounds->height() + d_top + d_btm > _worldDimension.getHeight()) {
            d_top = 0;
            d_btm = _worldDimension.getHeight() - _bounds->height();
        }

        // Index out of bounds, but nowhere to grow!
        ASSERT(d_lft + d_rgt + d_top + d_btm != 0, "Invalid plate growth deltas");

        const uint32_t old_width = _bounds->width();
        const uint32_t old_height = _bounds->height();

        _bounds->shift(-1.0f * d_lft, -1.0f * d_top);
        _bounds->grow(d_lft + d_rgt, d_top + d_btm);

        HeightMap tmph = HeightMap(_bounds->width(), _bounds->height());
        AgeMap tmpa = AgeMap(_bounds->width(), _bounds->height());
        uint32_t* tmps = new uint32_t[_bounds->area()];
        tmph.set_all(0);
        tmpa.set_all(0);
        memset(tmps, 255, _bounds->area() * sizeof(uint32_t));

        // copy old plate into new.
        for (uint32_t j = 0; j < old_height; ++j) {
            const uint32_t dest_i = (d_top + j) * _bounds->width() + d_lft;
            const uint32_t src_i = j * old_width;
            memcpy(&tmph[dest_i], &map[src_i], old_width * sizeof(float));
            memcpy(&tmpa[dest_i], &age_map[src_i], old_width * sizeof(uint32_t));
            memcpy(&tmps[dest_i], &_segments->id(src_i), old_width * sizeof(uint32_t));
        }

        map = tmph;
        age_map = tmpa;
        _segments->reassign(_bounds->area(), tmps);

        // Shift all segment data to match new coordinates.
        _segments->shift(d_lft, d_top);

        _x = x, _y = y;
        index = _bounds->getValidMapIndex(&_x, &_y);

        assert(index < _bounds->area());
    }

    // Update crust's age.
    // If old crust exists, new age is mean of original and supplied ages.
    // If no new crust is added, original time remains intact.
    const uint32_t old_crust = -(map[index] > 0);
    const uint32_t new_crust = -(z > 0);
    t = (t & ~old_crust) |
        ((uint32_t)((map[index] * age_map[index] + z * t) / (map[index] + z)) & old_crust);
    age_map[index] = (t & new_crust) | (age_map[index] & ~new_crust);

    // Clamp to prevent floating point precision errors (Issue #30)
    if (z < 0.0f) {
        z = 0.0f;
    }

    _mass.incMass(-1.0f * map[index]);
    _mass.incMass(z); // Update mass counter.
    map[index] = z;   // Set new crust height to desired location.
}

ContinentId plate::selectCollisionSegment(uint32_t coll_x, uint32_t coll_y) {
    uint32_t index = _bounds->getValidMapIndex(&coll_x, &coll_y);
    ContinentId activeContinent = _segments->id(index);
    return activeContinent;
}

///////////////////////////////////////////////////////////////////////////////
/// Private methods ///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

uint32_t plate::createSegment(uint32_t x, uint32_t y) noexcept {
    return _mySegmentCreator->createSegment(x, y);
}

ISegmentData& plate::getContinentAt(int x, int y) {
    return (*_segments)[_segments->getContinentAt(x, y)];
}

const ISegmentData& plate::getContinentAt(int x, int y) const {
    return (*_segments)[_segments->getContinentAt(x, y)];
}
