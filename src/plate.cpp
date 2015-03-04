/******************************************************************************
 *  PlaTec, a 2D terrain generator based on plate tectonics
 *  Copyright (C) 2012- Lauri Viitanen
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
#include <cfloat>    // FT_EPSILON
#ifdef __MINGW32__ // this is to avoid a problem with the hypot function which is messed up by Python...
#undef __STRICT_ANSI__
#endif
#include <cmath>     // sin, cos
#include <cstdlib>   // rand
#include <vector>
#include <stdexcept> // std::invalid_argument
#include <assert.h>

#include "plate.hpp"
#include "heightmap.hpp"
#include "rectangle.hpp"
#include "utils.hpp"
#include "plate_functions.hpp"

using namespace std;

plate::plate(long seed, const float* m, uint32_t w, uint32_t h, uint32_t _x, uint32_t _y,
             uint32_t plate_age, WorldDimension worldDimension) :
             _randsource(seed),
             _mass(m, Bounds(worldDimension, FloatPoint(_x, _y), Dimension(w, h))),
             _bounds(worldDimension, FloatPoint(_x, _y), Dimension(w, h)),
             map(w, h), 
             age_map(w, h), 
             _worldDimension(worldDimension),
             _movement(_randsource, worldDimension),
             _segments(w * h)
{
    if (NULL == m) {
        throw invalid_argument("the given heightmap should not be null");
    }
    if (w <= 0 || h <= 0) {
        throw invalid_argument("width and height of the plate should be greater than zero");
    }
    if (_x < 0 || _y <0) {
        throw invalid_argument("coordinates of the plate should be greater or equal to zero");
    }
    if (plate_age < 0) {
        throw invalid_argument("age of the plate should be greater or equal to zero");
    }

    const uint32_t plate_area = w * h;

    uint32_t k;
    for (uint32_t y = k = 0; y < _bounds.height(); ++y) {
        for (uint32_t x = 0; x < _bounds.width(); ++x, ++k) {
            // Clone map data and count crust mass.
            map[k] = m[k];

            // Set the age of ALL points in this plate to same
            // value. The right thing to do would be to simulate
            // the generation of new oceanic crust as if the plate
            // had been moving to its current direction until all
            // plate's (oceanic) crust receive an age.
            age_map.set(x, y, plate_age & -(m[k] > 0));
        }
    }
}

uint32_t plate::addCollision(uint32_t wx, uint32_t wy)
{
    ContinentId seg = getContinentAt(wx, wy);
    _segments[seg].incCollCount();
    return _segments[seg].area();
}

void plate::addCrustByCollision(uint32_t x, uint32_t y, float z, uint32_t time, ContinentId activeContinent)
{
    // Add crust. Extend plate if necessary.
    setCrust(x, y, getCrust(x, y) + z, time);

    uint32_t index = _bounds.getValidMapIndex(&x, &y);
    _segments.setId(index, activeContinent);

    SegmentData& data = _segments[activeContinent];
    data.incArea();
    data.enlarge_to_contain(x, y);
}

void plate::addCrustBySubduction(uint32_t x, uint32_t y, float z, uint32_t t,
    float dx, float dy)
{
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
    uint32_t index = _bounds.getValidMapIndex(&x, &y);

    // Take vector difference only between plates that move more or less
    // to same direction. This makes subduction direction behave better.
    //
    // Use of "this" pointer is not necessary, but it make code clearer.
    // Cursed be those who use "m_" prefix in member names! >(
    float dot = _movement.dot(dx, dy);
    dx -= _movement.velocityOnX(dot > 0);
    dy -= _movement.velocityOnY(dot > 0);

    float offset = (float)_randsource.next_double();
    offset *= offset * offset * (2 * (_randsource.next() & 1) - 1);
    dx = 10 * dx + 3 * offset;
    dy = 10 * dy + 3 * offset;

    x = (uint32_t)((int)x + dx);
    y = (uint32_t)((int)y + dy);

    if (_bounds.width() == _worldDimension.getWidth()) {
        x %= _bounds.width();
    }
    if (_bounds.height() == _worldDimension.getHeight()) {
        y %= _bounds.height();
    }

    index = _bounds.index(x, y);
    if (index < _bounds.area() && map[index] > 0)
    {
        t = (map[index] * age_map[index] + z * t) / (map[index] + z);
        age_map[index] = t * (z > 0);

        map[index] += z;
        _mass.incMass(z);
    }
}

float plate::aggregateCrust(plate* p, uint32_t wx, uint32_t wy)
{ 
    uint32_t lx = wx, ly = wy;
    const uint32_t index = _bounds.getValidMapIndex(&lx, &ly);

    const ContinentId seg_id = _segments.id(index);

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
    if (_segments[seg_id].isEmpty()) {
        return 0;   // Do not process empty continents.
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
    for (uint32_t y = _segments[seg_id].getTop(); y <= _segments[seg_id].getBottom(); ++y)
    {
      for (uint32_t x = _segments[seg_id].getLeft(); x <= _segments[seg_id].getRight(); ++x)
      {
        const uint32_t i = y * _bounds.width() + x;
        if ((_segments.id(i) == seg_id) && (map[i] > 0))
        {
            p->addCrustByCollision(wx + x - lx, wy + y - ly,
                map[i], age_map[i], activeContinent);

            _mass.incMass(-1.0f * map[i]);
            map[i] = 0;
        }
      }
    }

    _segments[seg_id].markNonExistent(); // Mark segment as non-existent
    return old_mass - _mass.getMass();
}

void plate::applyFriction(float deformed_mass)
{
    // Remove the energy that deformation consumed from plate's kinetic
    // energy: F - dF = ma - dF => a = dF/m.
    if (_mass.notNull())
    {
        _movement.applyFriction(deformed_mass, _mass.getMass());
    }
}

void plate::collide(plate& p, uint32_t wx, uint32_t wy, float coll_mass)
{
    _movement.collide(*this, p, this->_movement, p._movement, wx, wy, coll_mass);
}

bool plate::contains(uint32_t x, uint32_t y) const
{
    return _bounds.contains(x, y);
}

void plate::calculateCrust(uint32_t x, uint32_t y, uint32_t index, 
    float& w_crust, float& e_crust, float& n_crust, float& s_crust,
    uint32_t& w, uint32_t& e, uint32_t& n, uint32_t& s)
{
    ::calculateCrust(x, y, index, w_crust, e_crust, n_crust, s_crust,
        w, e, n, s, 
        _worldDimension, map, _bounds.width(), _bounds.height());
}

void plate::findRiverSources(float lower_bound, vector<uint32_t>* sources)
{
  // Find all tops.
  for (uint32_t y = 0; y < _bounds.height(); ++y) {
    for (uint32_t x = 0; x < _bounds.width(); ++x) {
        const uint32_t index = _bounds.index(x, y);

        if (map[index] < lower_bound) {
            continue;
        }

        float w_crust, e_crust, n_crust, s_crust;
        uint32_t w, e, n, s;
        calculateCrust(x, y, index, w_crust, e_crust, n_crust, s_crust,
            w, e, n, s);

        // This location is either at the edge of the plate or it is not the
        // tallest of its neightbours. Don't start a river from here.
        if (w_crust * e_crust * n_crust * s_crust == 0) {
            continue;
        }

        sources->push_back(index);
    }
  }
}

void plate::flowRivers(float lower_bound, vector<uint32_t>* sources, float* tmp)
{
  vector<uint32_t> sinks_data;
  vector<uint32_t>* sinks = &sinks_data;

  uint32_t* isDone = new uint32_t[_bounds.area()];
  memset(isDone, 0, _bounds.area() * sizeof(uint32_t));

  // From each top, start flowing water along the steepest slope.
  while (!sources->empty()) {
    while (!sources->empty()) {
        const uint32_t index = sources->back();
        const uint32_t y = index / _bounds.width();
        const uint32_t x = index - y * _bounds.width();

        sources->pop_back();

        if (map[index] < lower_bound) {
            continue;
        }

        float w_crust, e_crust, n_crust, s_crust;
        uint32_t w, e, n, s;
        calculateCrust(x, y, index, w_crust, e_crust, n_crust, s_crust,
            w, e, n, s);

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
            dest = index - _bounds.width();
        }

        if (s_crust < lowest_crust) {
            lowest_crust = s_crust;
            dest = index + _bounds.width();
        }

        // if it's not handled yet, add it as new sink.
        if (dest < _bounds.area() && !isDone[dest]) {
            sinks->push_back(dest);
            isDone[dest] = 1;
        }

        // Erode this location with the water flow.
        tmp[index] -= (tmp[index] - lower_bound) * 0.2;
    }


    vector<uint32_t>* v_tmp = sources;
    sources = sinks;
    sinks = v_tmp;
    sinks->clear();
  }

  delete[] isDone;
}

void plate::erode(float lower_bound)
{
  vector<uint32_t> sources_data;  
  vector<uint32_t>* sources = &sources_data;  

  float* tmp = new float[_bounds.area()];
  map.copy_raw_to(tmp);

  findRiverSources(lower_bound, sources);

  flowRivers(lower_bound, sources, tmp);

  // Add random noise (10 %) to heightmap.
  for (uint32_t i = 0; i < _bounds.area(); ++i)
  {
    float alpha = 0.2 * (float)_randsource.next_double();
    tmp[i] += 0.1 * tmp[i] - alpha * tmp[i];
  }

  memcpy(map.raw_data(), tmp, _bounds.area()*sizeof(float));
  memset(tmp, 0, _bounds.area()*sizeof(float));
  _mass.reset();

  for (uint32_t y = 0; y < _bounds.height(); ++y)
    for (uint32_t x = 0; x < _bounds.width(); ++x)
    {
    const uint32_t index = y * _bounds.width() + x;
    _mass.addPoint(x, y, map[index]);    
    tmp[index] += map[index]; // Careful not to overwrite earlier amounts.

    if (map[index] < lower_bound)
        continue;

    float w_crust, e_crust, n_crust, s_crust;
    uint32_t w, e, n, s;
    calculateCrust(x, y, index, w_crust, e_crust, n_crust, s_crust,
        w, e, n, s);

    // This location has no neighbours (ARTIFACT!) or it is the lowest
    // part of its area. In either case the work here is done.
    if (w_crust + e_crust + n_crust + s_crust == 0)
        continue;

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
    float diff_sum = (w_diff - min_diff) * (w_crust > 0) +
                     (e_diff - min_diff) * (e_crust > 0) +
                     (n_diff - min_diff) * (n_crust > 0) +
                     (s_diff - min_diff) * (s_crust > 0);

    // Erosion difference sum is negative!
    assert(diff_sum >= 0);

    if (diff_sum < min_diff)
    {
        // There's NOT enough room in neighbours to contain all the
        // crust from this peak so that it would be as tall as its
        // tallest lower neighbour. Thus first step is make ALL
        // lower neighbours and this point equally tall.
        tmp[w] += (w_diff - min_diff) * (w_crust > 0);
        tmp[e] += (e_diff - min_diff) * (e_crust > 0);
        tmp[n] += (n_diff - min_diff) * (n_crust > 0);
        tmp[s] += (s_diff - min_diff) * (s_crust > 0);
        tmp[index] -= min_diff;

        min_diff -= diff_sum;

        // Spread the remaining crust equally among all lower nbours.
        min_diff /= 1 + (w_crust > 0) + (e_crust > 0) +
            (n_crust > 0) + (s_crust > 0);

        tmp[w] += min_diff * (w_crust > 0);
        tmp[e] += min_diff * (e_crust > 0);
        tmp[n] += min_diff * (n_crust > 0);
        tmp[s] += min_diff * (s_crust > 0);
        tmp[index] += min_diff;
    }
    else
    {
        float unit = min_diff / diff_sum;

        // Remove all crust from this location making it as tall as
        // its tallest lower neighbour.
        tmp[index] -= min_diff;

        // Spread all removed crust among all other lower neighbours.
        tmp[w] += unit * (w_diff - min_diff) * (w_crust > 0);
        tmp[e] += unit * (e_diff - min_diff) * (e_crust > 0);
        tmp[n] += unit * (n_diff - min_diff) * (n_crust > 0);
        tmp[s] += unit * (s_diff - min_diff) * (s_crust > 0);
    }
    }

  map.from(tmp);

  _mass.redistribute();
}

void plate::getCollisionInfo(uint32_t wx, uint32_t wy, uint32_t* count, float* ratio) const
{
    ContinentId seg = getContinentAt(wx, wy);

    *count = 0;
    *ratio = 0;

    *count = _segments[seg].collCount();
    *ratio = (float)_segments[seg].collCount() /
        (float)(1 + _segments[seg].area()); // +1 avoids DIV with zero.
}

uint32_t plate::getContinentArea(uint32_t wx, uint32_t wy) const
{
    const uint32_t index = _bounds.getValidMapIndex(&wx, &wy);
    assert(_segments.id(index) < _segments.size());
    return _segments[_segments.id(index)].area(); 
}

float plate::getCrust(uint32_t x, uint32_t y) const
{
    const uint32_t index = _bounds.getMapIndex(&x, &y);
    return index != BAD_INDEX ? map[index] : 0;        
}

uint32_t plate::getCrustTimestamp(uint32_t x, uint32_t y) const
{
    const uint32_t index = _bounds.getMapIndex(&x, &y);
    return index != BAD_INDEX ? age_map[index] : 0; 
}

void plate::getMap(const float** c, const uint32_t** t) const
{
    if (c) {
        *c = map.raw_data();
    }
    if (t) {
        *t = age_map.raw_data();
    }
}

void plate::move()
{
    _movement.move();

    // Location modulations into range [0..world width/height[ are a have to!
    // If left undone SOMETHING WILL BREAK DOWN SOMEWHERE in the code!

    _bounds.grow(_movement.velocityOnX(), _movement.velocityOnY());
}

void plate::resetSegments()
{
    p_assert(_bounds.area()==_segments.area(), "Segments has not the expected area");
    _segments.reset();
}

void plate::setCrust(uint32_t x, uint32_t y, float z, uint32_t t)
{
    if (z < 0) { // Do not accept negative values.
        z = 0;
    }

    uint32_t _x = x;
    uint32_t _y = y;
    uint32_t index = _bounds.getMapIndex(&_x, &_y);

    if (index == BAD_INDEX)
    {
        // Extending plate for nothing!
        assert(z>0);

        const uint32_t ilft = _bounds.left();
        const uint32_t itop = _bounds.top();
        const uint32_t irgt = _bounds.right();
        const uint32_t ibtm = _bounds.bottom();

        _worldDimension.normalize(x, y);

        // Calculate distance of new point from plate edges.
        const uint32_t _lft = ilft - x;
        const uint32_t _rgt = (_worldDimension.getWidth() & -(x < ilft)) + x - irgt;
        const uint32_t _top = itop - y;
        const uint32_t _btm = (_worldDimension.getHeight() & -(y < itop)) + y - ibtm;

        // Set larger of horizontal/vertical distance to zero.
        // A valid distance is NEVER larger than world's side's length!
        uint32_t d_lft = _lft & -(_lft <  _rgt) & -(_lft < _worldDimension.getWidth());
        uint32_t d_rgt = _rgt & -(_rgt <= _lft) & -(_rgt < _worldDimension.getWidth());
        uint32_t d_top = _top & -(_top <  _btm) & -(_top < _worldDimension.getHeight());
        uint32_t d_btm = _btm & -(_btm <= _top) & -(_btm < _worldDimension.getHeight());

        // Scale all changes to multiple of 8.
        d_lft = ((d_lft > 0) + (d_lft >> 3)) << 3;
        d_rgt = ((d_rgt > 0) + (d_rgt >> 3)) << 3;
        d_top = ((d_top > 0) + (d_top >> 3)) << 3;
        d_btm = ((d_btm > 0) + (d_btm >> 3)) << 3;

        // Make sure plate doesn't grow bigger than the system it's in!
        if (_bounds.width() + d_lft + d_rgt > _worldDimension.getWidth())
        {
            d_lft = 0;
            d_rgt = _worldDimension.getWidth() - _bounds.width();
        }

        if (_bounds.height() + d_top + d_btm > _worldDimension.getHeight())
        {
            d_top = 0;
            d_btm = _worldDimension.getHeight() - _bounds.height();
        }

        // Index out of bounds, but nowhere to grow!
        assert(d_lft + d_rgt + d_top + d_btm != 0);

        const uint32_t old_width  = _bounds.width();
        const uint32_t old_height = _bounds.height();
        
        _bounds.grow(-1.0*d_lft, -1.0*d_top);
        _bounds.growWidth(d_lft + d_rgt);
        _bounds.growHeight(d_top + d_btm);

        HeightMap tmph = HeightMap(_bounds.width(), _bounds.height());
        AgeMap    tmpa = AgeMap(_bounds.width(), _bounds.height());
        uint32_t* tmps = new uint32_t[_bounds.area()];
        tmph.set_all(0);
        tmpa.set_all(0);
        memset(tmps, 255, _bounds.area()*sizeof(uint32_t));

        // copy old plate into new.
        for (uint32_t j = 0; j < old_height; ++j)
        {
            const uint32_t dest_i = (d_top + j) * _bounds.width() + d_lft;
            const uint32_t src_i = j * old_width;
            memcpy(&tmph[dest_i], &map[src_i], old_width *
                sizeof(float));
            memcpy(&tmpa[dest_i], &age_map[src_i], old_width *
                sizeof(uint32_t));
            memcpy(&tmps[dest_i], &_segments.id(src_i), old_width *
                sizeof(uint32_t));
        }
        
        map     = tmph;
        age_map = tmpa;
        _segments.reassign(_bounds.area(), tmps);

        // Shift all segment data to match new coordinates.
        _segments.shift(d_lft, d_top);

        _x = x, _y = y;
        index = _bounds.getValidMapIndex(&_x, &_y);

        assert(index < _bounds.area());
    }

    // Update crust's age.
    // If old crust exists, new age is mean of original and supplied ages.
    // If no new crust is added, original time remains intact.
    const uint32_t old_crust = -(map[index] > 0);
    const uint32_t new_crust = -(z > 0);
    t = (t & ~old_crust) | ((uint32_t)((map[index] * age_map[index] + z * t) /
        (map[index] + z)) & old_crust);
    age_map[index] = (t & new_crust) | (age_map[index] & ~new_crust);

    _mass.incMass(-1.0f * map[index]);
    _mass.incMass(z);      // Update mass counter.
    map[index] = z;     // Set new crust height to desired location.    
}

ContinentId plate::selectCollisionSegment(uint32_t coll_x, uint32_t coll_y)
{
    uint32_t index = _bounds.getValidMapIndex(&coll_x, &coll_y);
    ContinentId activeContinent = _segments.id(index);
    return activeContinent;   
}

///////////////////////////////////////////////////////////////////////////////
/// Private methods ///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

uint32_t plate::calcDirection(uint32_t x, uint32_t y, const uint32_t origin_index, const uint32_t ID)
{
    uint32_t canGoLeft  = x > 0          && map[origin_index - 1]     >= CONT_BASE;
    uint32_t canGoRight = x < _bounds.width() - 1  && map[origin_index+1]       >= CONT_BASE;
    uint32_t canGoUp    = y > 0          && map[origin_index - _bounds.width()] >= CONT_BASE;
    uint32_t canGoDown  = y < _bounds.height() - 1 && map[origin_index + _bounds.width()] >= CONT_BASE;
    uint32_t nbour_id = ID;

    // This point belongs to no segment yet.
    // However it might be a neighbour to some segment created earlier.
    // If such neighbour is found, associate this point with it.
    if (canGoLeft && _segments.id(origin_index - 1) < ID) {
        nbour_id = _segments.id(origin_index - 1);
    } else if (canGoRight && _segments.id(origin_index + 1) < ID) {
        nbour_id = _segments.id(origin_index + 1);
    } else if (canGoUp && _segments.id(origin_index - _bounds.width()) < ID) {
        nbour_id = _segments.id(origin_index - _bounds.width());
    } else if (canGoDown && _segments.id(origin_index + _bounds.width()) < ID) {
        nbour_id = _segments.id(origin_index + _bounds.width());
    }

    return nbour_id;
}

void plate::scanSpans(const uint32_t line, uint32_t& start, uint32_t& end,
    std::vector<uint32_t>* spans_todo, std::vector<uint32_t>* spans_done)
{
    do // Find an unscanned span on this line.
    {
        end = spans_todo[line].back();
        spans_todo[line].pop_back();

        start = spans_todo[line].back();
        spans_todo[line].pop_back();

        // Reduce any done spans from this span.
        for (uint32_t j = 0; j < spans_done[line].size();
             j += 2)
        {
            // Saved coordinates are AT the point
            // that was included last to the span.
            // That's why equalities matter.

            if (start >= spans_done[line][j] &&
                start <= spans_done[line][j+1])
                start = spans_done[line][j+1] + 1;

            if (end >= spans_done[line][j] &&
                end <= spans_done[line][j+1])
                end = spans_done[line][j] - 1;
        }

        // Unsigned-ness hacking!
        // Required to fix the underflow of end - 1.
        start |= -(end >= _bounds.width());
        end -= (end >= _bounds.width());

    } while (start > end && spans_todo[line].size());
}

uint32_t plate::createSegment(uint32_t x, uint32_t y) throw()
{
    const uint32_t origin_index = _bounds.index(x, y);
    const uint32_t ID = _segments.size();

    if (_segments.id(origin_index) < ID) {
        return _segments.id(origin_index);
    }

    uint32_t nbour_id = calcDirection(x, y, origin_index, ID);

    if (nbour_id < ID)
    {
        _segments.setId(origin_index, nbour_id);
        _segments[nbour_id].incArea();

        _segments[nbour_id].enlarge_to_contain(x, y);

        return nbour_id;
    }

    uint32_t lines_processed;
    Platec::Rectangle r = Platec::Rectangle(_worldDimension, x, x, y, y);
    SegmentData data(r, 0);

    std::vector<uint32_t>* spans_todo = new std::vector<uint32_t>[_bounds.height()];
    std::vector<uint32_t>* spans_done = new std::vector<uint32_t>[_bounds.height()];

    _segments.setId(origin_index, ID);
    spans_todo[y].push_back(x);
    spans_todo[y].push_back(x);

    do
    {
      lines_processed = 0;
      for (uint32_t line = 0; line < _bounds.height(); ++line)
      {
        uint32_t start, end;

        if (spans_todo[line].size() == 0)
            continue;

        scanSpans(line, start, end, spans_todo, spans_done);

        if (start > end) // Nothing to do here anymore...
            continue;

        // Calculate line indices. Allow wrapping around map edges.
        const uint32_t row_above = ((line - 1) & -(line > 0)) |
            ((_bounds.height() - 1) & -(line == 0));
        const uint32_t row_below = (line + 1) & -(line < _bounds.height() - 1);
        const uint32_t line_here = line * _bounds.width();
        const uint32_t line_above = row_above * _bounds.width();
        const uint32_t line_below = row_below * _bounds.width();

        // Extend the beginning of line.
        while (start > 0 && _segments.id(line_here+start-1) > ID &&
            map[line_here+start-1] >= CONT_BASE)
        {
            --start;
            _segments.setId(line_here + start, ID);

            // Count volume of pixel...
        }

        // Extend the end of line.
        while (end < _bounds.width() - 1 &&
            _segments.id(line_here + end + 1) > ID &&
            map[line_here + end + 1] >= CONT_BASE)
        {
            ++end;
            _segments.setId(line_here + end, ID);

            // Count volume of pixel...
        }

        // Check if should wrap around left edge.
        if (_bounds.width() == _worldDimension.getWidth() && start == 0 &&
            _segments.id(line_here+_bounds.width()-1) > ID &&
            map[line_here+_bounds.width()-1] >= CONT_BASE)
        {
            _segments.setId(line_here + _bounds.width() - 1, ID);
            spans_todo[line].push_back(_bounds.width() - 1);
            spans_todo[line].push_back(_bounds.width() - 1);

            // Count volume of pixel...
        }

        // Check if should wrap around right edge.
        if (_bounds.width() == _worldDimension.getWidth() && end == _bounds.width() - 1 &&
            _segments.id(line_here+0) > ID &&
            map[line_here+0] >= CONT_BASE)
        {
            _segments.setId(line_here + 0, ID);
            spans_todo[line].push_back(0);
            spans_todo[line].push_back(0);

            // Count volume of pixel...
        }

        data.incArea(1 + end - start); // Update segment area counter.

        // Record any changes in extreme dimensions.
        if (line < data.getTop()) data.setTop(line);
        if (line > data.getBottom()) data.setBottom(line);
        if (start < data.getLeft()) data.setLeft(start);
        if (end > data.getRight()) data.setRight(end);

        if (line > 0 || _bounds.height() == _worldDimension.getHeight()) {
            for (uint32_t j = start; j <= end; ++j)
              if (_segments.id(line_above + j) > ID &&
                  map[line_above + j] >= CONT_BASE)
              {
                uint32_t a = j;
                _segments.setId(line_above + a, ID);

                // Count volume of pixel...

                while (++j < _bounds.width() &&
                       _segments.id(line_above + j) > ID &&
                       map[line_above + j] >= CONT_BASE)
                {
                    _segments.setId(line_above + j, ID);

                    // Count volume of pixel...
                }

                uint32_t b = --j; // Last point is invalid.

                spans_todo[row_above].push_back(a);
                spans_todo[row_above].push_back(b);
                ++j; // Skip the last scanned point.
              }
        }

        if (line < _bounds.height() - 1 || _bounds.height() == _worldDimension.getHeight()) {
            for (uint32_t j = start; j <= end; ++j)
              if (_segments.id(line_below + j) > ID &&
                  map[line_below + j] >= CONT_BASE)
              {
                uint32_t a = j;
                _segments.setId(line_below + a, ID);

                // Count volume of pixel...

                while (++j < _bounds.width() &&
                       _segments.id(line_below + j) > ID &&
                       map[line_below + j] >= CONT_BASE)
                {
                    _segments.setId(line_below + j, ID);

                    // Count volume of pixel...
                }

                uint32_t b = --j; // Last point is invalid.

                spans_todo[row_below].push_back(a);
                spans_todo[row_below].push_back(b);
                ++j; // Skip the last scanned point.
              }
        }

        spans_done[line].push_back(start);
        spans_done[line].push_back(end);
        ++lines_processed;
      }
    } while (lines_processed > 0);

    delete[] spans_todo;
    delete[] spans_done;
    _segments.add(data);

    return ID;
}

ContinentId plate::getContinentAt(int x, int y) const
{
    uint32_t lx = x, ly = y;
    uint32_t index = _bounds.getValidMapIndex(&lx, &ly);
    ContinentId seg = _segments.id(index);

    if (seg >= _segments.size()) {
        // in this case, we consider as const this call because we calculate
        // something that we would calculate anyway, so the segments are
        // a sort of cache
        seg = const_cast<plate*>(this)->createSegment(lx, ly);
    }

    if (seg >= _segments.size())
    {
        throw invalid_argument("Could not create segment");
    }
    return seg;  
}
