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
#include "utils.hpp"
#include "world_properties.h"


plate::plate( const long seed,const HeightMap&  m, 
        const Dimension& plateDimension,
            const Platec::vec2f& topLeftCorner,
         const uint32_t index) :
    randsource(seed),
    map(m),        
    age_map(plateDimension), 
    mass(MassBuilder(m).build()),
    movement(randsource),
    worldDimension(world_properties::get().getWorldDimension()),
        index(index)
{
    const uint32_t plate_area = plateDimension.getArea();

    bounds = std::make_shared<Bounds>(topLeftCorner, plateDimension);

    
    auto mapItr = map.getData().begin();
    
    std::replace_if(age_map.getData().begin(),age_map.getData().end(),
                    [&](auto)
                    {
                        auto ret = *mapItr != 0;
                        ++mapItr;
                        return ret;
                    }, index); //use index als default plate age


    segments = std::make_shared<Segments>(plate_area);
    mySegmentCreator = std::make_shared<MySegmentCreator>(bounds, segments, map);
    segments->setSegmentCreator(mySegmentCreator);
    segments->setBounds(bounds);
}

void plate::setIndex(uint32_t index) {
    this->index = index;
}

uint32_t plate::getIndex() const {
    return index;
}


uint32_t plate::addCollision(const Platec::vec2ui& point)
{
    auto& seg = getContinentAt(point);
    seg.incCollCount();
    return seg.getArea();
}

void plate::addCrustByCollision(const Platec::vec2ui& point,const float_t z,
                            const uint32_t time,const ContinentId activeContinent)
{
    // Add crust. Extend plate if necessary.
    setCrust(point, getCrust(point) + z, time);

    auto index = bounds->getValidMapIndex(point);
    segments->setId(index.first, activeContinent);

    ISegmentData& data = segments->getSegmentData(activeContinent);
    data.incArea();
    data.enlarge_to_contain(index.second);
}

void plate::addCrustBySubduction(const Platec::vec2ui& originPoint,const float_t sediment,const uint32_t time,
                              const Platec::vec2f& dir)
{
    if(sediment <= 0) //should be checked earlier
    {
        return;
    }
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


    // Take vector difference only between plates that move more or less
    // to same direction. This makes subduction direction behave better.
    //
    // Use of "this" pointer is not necessary, but it make code clearer.
    // Cursed be those who use "m_" prefix in member names! >(
    auto dotDir = dir;
    if(movement.dot(dir) > 0)
    {
        dotDir = dotDir - movement.velocityVector();
    }
    
    //What the hell are we doing here? Why have we to calculete 10 * x +3 ????
    auto offset =std::pow((float_t)randsource.next_double(),3);
     offset = std::copysign(offset,  2 * (int)(randsource.next() % 2) - 1); 
    
    auto offset2 =std::copysign(std::pow((float_t)randsource.next_double(),3),
                                                    randsource.next_signed()); 
    
    dotDir = Platec::vec2f(10 * dotDir.x() + 3 * offset,10 * dotDir.y() + 3 * offset2);

    const auto p =  Platec::vec2ui(dotDir.x(),dotDir.y()) + bounds->getValidMapIndex(originPoint).second; 
    if (bounds->isInLimits(p) )
    {
        
        uint32_t tmpindex = bounds->index(p);
        auto mapItr = map.getData().begin() + tmpindex;
        auto ageMapItr = age_map.getData().begin() + tmpindex;

        if (*mapItr > 0 )
        {

            *ageMapItr = ((*mapItr) * (*ageMapItr) + sediment * time) / ((*mapItr) + sediment);

            (*mapItr) += sediment;
            mass.incMass(sediment);
        }
    }
}

float plate::aggregateCrust(plate& p, Platec::vec2ui point)
{

    const auto index = bounds->getValidMapIndex(point);

    const ContinentId seg_id = segments->id(index.first);

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
    if (segments->getSegmentData(seg_id).isEmpty()) {
        return 0;   // Do not process empty continents.
    }

    ContinentId activeContinent = p.selectCollisionSegment(point);

    // Wrap coordinates around world edges to safeguard subtractions.
    point = point + worldDimension.getDimensions();

    // Aggregating segment [%u, %u]x[%u, %u] vs. [%u, %u]@[%u, %u]\n",
    //      seg_data[seg_id].x0, seg_data[seg_id].y0,
    //      seg_data[seg_id].x1, seg_data[seg_id].y1,
    //      _dimension.getWidth(), _dimension.getHeight(), lx, ly);

    float old_mass = mass.getMass();

    // Add all of the collided continent's crust to destination plate.
    
    
    for (uint32_t y = segments->getSegmentData(seg_id).getTop(); y <= segments->getSegmentData(seg_id).getBottom(); ++y)
    {
        for (uint32_t x = segments->getSegmentData(seg_id).getLeft(); x <= segments->getSegmentData(seg_id).getRight(); ++x)
        {
            const uint32_t i = bounds->getDimension().indexOf(Platec::vec2ui(x,y));
            if ((segments->id(i) == seg_id) && (map[i] > 0))
            {
                p.addCrustByCollision(point + Platec::vec2ui(x,y)- index.second,
                                       map[i], age_map[i], activeContinent);

                mass.incMass(-1.0f * map[i]);
                map[i] = 0.0f;
            }
        }
    }

    segments->getSegmentData(seg_id).markNonExistent(); // Mark segment as non-existent
    return old_mass - mass.getMass();
}

void plate::applyFriction(const float_t deformed_mass)
{
    // Remove the energy that deformation consumed from plate's kinetic
    // energy: F - dF = ma - dF => a = dF/m.
    if (!mass.isNull())
    {
        movement.applyFriction(deformed_mass, mass.getMass());
    }
}

void plate::collide(plate& p,const float_t coll_mass)
{
    if (!mass.isNull() && coll_mass > 0) {
        movement.collide(mass, p, coll_mass);
    }
}

const surroundingPoints plate::calculateCrust(const uint32_t index) const
{
    const auto& position = bounds->getDimension().coordOF(index);
    const auto& position_world = worldDimension.pointMod(position);
    const auto height = map.get(index);
    auto ret = surroundingPoints();

    ret.centerIndex = index;
    if(position.x() > 0 || bounds->width()==worldDimension.getWidth() )
    {
        if(position_world.x() == 0)
        {
            ret.westIndex = bounds->index(Platec::vec2ui(worldDimension.getWidth() -1, position.y()));
        }
        else
        {
            ret.westIndex = bounds->index(Platec::vec2ui(position_world.x()- 1,position.y()));
        }
        if(map[ret.westIndex] < height)
        {
          ret.westCrust = map[ret.westIndex];
        }
    }

    
    if(position.x() < bounds->width()-1 || bounds->width()==worldDimension.getWidth() )
    {
        if(position_world.x()+1 == worldDimension.getWidth())
        {
            ret.eastIndex = bounds->index(Platec::vec2ui(0, position.y()));
        }
        else
        {
            ret.eastIndex = bounds->index(Platec::vec2ui(position_world.x()+ 1,position.y()));
        }        
        if(map[ret.eastIndex] < height)
        {
          ret.eastCrust = map[ret.eastIndex];
        }
    }


    
    if(position.y() > 0 || bounds->height()==worldDimension.getHeight() )
    {
        if(position_world.y() == 0)
        {
            ret.northIndex = bounds->index(Platec::vec2ui( position.x(),worldDimension.getHeight() -1));
        }
        else
        {
            ret.northIndex = bounds->index(Platec::vec2ui( position.x(),position_world.y() -1));
        } 
        if(map[ret.northIndex] < height)
        {
          ret.northCrust = map[ret.northIndex];
        }
    }

      
    if(position.y() < bounds->height()-1 || bounds->height()==worldDimension.getHeight() )
    {
        if(position_world.y()+1 == worldDimension.getHeight())
        {
            ret.southIndex = bounds->index(Platec::vec2ui(position.x(),0));
        }
        else
        {
            ret.southIndex = bounds->index(Platec::vec2ui( position.x(),position_world.y() +1));
        }           
        if(map[ret.southIndex] < height)
        {
          ret.southCrust = map[ret.southIndex];
        }
    }


    return ret;
}

std::vector<surroundingPoints> plate::findRiverSources(const float_t lower_bound)
{
    std::vector<surroundingPoints> sources = std::vector<surroundingPoints>(30);
    uint32_t index = 0;
    surroundingPoints tmp;
    for(const auto& val : map.getData())
    {
        tmp = calculateCrust(index);
        if(val >= lower_bound && !tmp.centerIsLowest())
        {
            sources.emplace_back(tmp);
        }
        ++index;
    }
    return sources;
}

std::vector<uint32_t> plate::flowRivers( std::vector<surroundingPoints> sources, std::vector<uint32_t> foundIndices)
{
    auto newSources = std::vector<surroundingPoints>();
    newSources.reserve(50);    
    
    while(!sources.empty())
    {
        newSources.clear();
        for(auto& val : sources)
        {
            if(val.oneIsLower()) // if center is not the lowest
            {
                newSources.emplace_back(calculateCrust(val.getLowestIndex())); //add lowest neighbor as source
                foundIndices.emplace_back(val.centerIndex);
            }   
        }
        sources.swap(newSources);
    }
    return foundIndices;
}

void plate::erode(float lower_bound)
{
    HeightMap tmpHm(bounds->getDimension());
    
    if(world_properties::get().isRiver_erosion_enable())
    {
        auto sinks = flowRivers(findRiverSources(lower_bound));
       //remove duplicates
        std::sort( sinks.begin(), sinks.end() );
        sinks.erase( std::unique( sinks.begin(), sinks.end() ), sinks.end() );
        for(const auto& index : sinks)
        {
            map[index] -= (map[index] - lower_bound) * world_properties::get().getRiver_erosion_strength();
        }
    }

    // Add random noise (10 %) to heightmap.
    if(world_properties::get().isNoise_enabel())
    {
        for(auto& val : map.getData())
        {
            val *= world_properties::get().getNoise_strength() - (0.2* (float)randsource.next_double());
        }
    }



    MassBuilder massBuilder;

    for (uint32_t index = 0; index < bounds->area(); ++index)
    {
        massBuilder.addPoint(bounds->getDimension().coordOF(index), map[index]);
        tmpHm[index] += map[index]; // Careful not to overwrite earlier amounts.

        if (map[index] < lower_bound)
            continue;

        surroundingPoints neighbors = calculateCrust(index);

        // This location has no neighbours (ARTIFACT!) or it is the lowest
        // part of its area. In either case the work here is done.
        if (neighbors.centerIsLowest())
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
        float w_diff = map[index] - neighbors.westCrust;
        float e_diff = map[index] - neighbors.eastCrust;
        float n_diff = map[index] - neighbors.northCrust;
        float s_diff = map[index] - neighbors.southCrust;


        float min_diff = std::min({w_diff,e_diff,n_diff,s_diff});

        // Calculate the sum of difference between lower neighbours and
        // the TALLEST lower neighbour.
        float diff_sum = (w_diff - min_diff) * (neighbors.westCrust > 0) +
                         (e_diff - min_diff) * (neighbors.eastCrust > 0) +
                         (n_diff - min_diff) * (neighbors.northCrust > 0) +
                         (s_diff - min_diff) * (neighbors.southCrust > 0);

        // Erosion difference sum is negative!
        ASSERT(diff_sum >= 0, "Difference sum must be positive");

        if (diff_sum < min_diff)
        {
            // There's NOT enough room in neighbours to contain all the
            // crust from this peak so that it would be as tall as its
            // tallest lower neighbour. Thus first step is make ALL
            // lower neighbours and this point equally tall.
            float median_min_diff = (min_diff - diff_sum)/( 1 + (neighbors.westCrust > 0) + (neighbors.eastCrust > 0) +
                        (neighbors.northCrust > 0) + (neighbors.southCrust > 0)) * world_properties::get().getDiffuse_strength();
            if(neighbors.westCrust > 0)
            {
                tmpHm[neighbors.westIndex ] += (w_diff - min_diff) + median_min_diff;
            }
            if(neighbors.eastCrust > 0)
            {
                tmpHm[neighbors.eastIndex ] += (e_diff - min_diff) + median_min_diff;
            }
            if(neighbors.northCrust > 0)
            {
                tmpHm[neighbors.northIndex] += (n_diff - min_diff) + median_min_diff;
            }
            if(neighbors.southCrust > 0 )
            {
                tmpHm[neighbors.southIndex] += (s_diff - min_diff) + median_min_diff;
            }
            tmpHm[index] = tmpHm[index] - min_diff + median_min_diff;
        }
        else
        {
            float unit = (min_diff / diff_sum) * world_properties::get().getDiffuse_strength();

            // Remove all crust from this location making it as tall as
            // its tallest lower neighbour.
            tmpHm[index] -= min_diff;

            // Spread all removed crust among all other lower neighbours.
            if(neighbors.westCrust > 0)
            {
                tmpHm[neighbors.westIndex ] += unit * (w_diff - min_diff);
            }
            if(neighbors.eastCrust > 0)
            {
                tmpHm[neighbors.eastIndex ] += unit * (e_diff - min_diff);
            }
            if(neighbors.northCrust > 0)
            {
                tmpHm[neighbors.northIndex] += unit * (n_diff - min_diff);
            }
            if(neighbors.southCrust > 0)
            {
               tmpHm[neighbors.southIndex] += unit * (s_diff - min_diff);
            }
        }
    }
    map = tmpHm;
    mass = massBuilder.build();
}

const std::pair<uint32_t,float_t> plate::getCollisionInfo(const Platec::vec2ui& point) const
{
    const ISegmentData& seg = getContinentAt(point);

    if(seg.getArea() == 0)
    {
        return  std::make_pair(seg.collCount(),seg.collCount());
    }
    return std::make_pair(seg.collCount(),seg.collCount() /(float_t)(seg.getArea()));


}

uint32_t plate::getContinentArea(const Platec::vec2ui& point) const
{
    const auto index = bounds->getValidMapIndex(point);
    ASSERT(segments->id(index.first) < segments->size(), "Segment index invalid");
    return segments->getSegmentData(segments->id(index.first)).getArea();
}

float_t plate::getCrust(const Platec::vec2ui& point) const
{
    const auto index = bounds->getMapIndex(point);
    return index.first != BAD_INDEX ? map[index.first] : 0;
}

uint32_t plate::getCrustTimestamp(const Platec::vec2ui& point) const
{
    const auto index = bounds->getMapIndex(point);
    return index.first != BAD_INDEX ? age_map[index.first] : 0;
}



AgeMap& plate::getAgeMap() {
    return age_map;
}

HeightMap& plate::getHeigthMap() {
    return map;
}


void plate::move()
{
    movement.move(worldDimension);
    bounds->shift(movement.velocityVector());
}

void plate::resetSegments()
{
    ASSERT(bounds->area() == segments->getArea(), "Segments doesn't have the expected area");
    segments->reset();
}

void plate::setCrust(const Platec::vec2ui& point, float_t z, uint32_t t)
{
    
    // Do not accept negative values.
    z = std::max(0.f,z);


    auto index = bounds->getMapIndex(point);

    if (index.first == BAD_INDEX)
    {
        // Extending plate for nothing!
        ASSERT(z > 0, "Height value must be non-zero");

        const uint32_t ilft = bounds->left();
        const uint32_t itop = bounds->top();
        const uint32_t irgt = bounds->rightAsUintNonInclusive();
        const uint32_t ibtm = bounds->bottomAsUintNonInclusive();
        
        auto normPoint = worldDimension.normalize(point);
        
        // Calculate distance of new point from plate edges.
        const uint32_t _lft = ilft - normPoint.x();
        const uint32_t _rgt = (worldDimension.getWidth() & -(normPoint.x() < ilft)) + normPoint.x() - irgt;
        const uint32_t _top = itop - normPoint.y();
        const uint32_t _btm = (worldDimension.getHeight() & -(normPoint.y() < itop)) + normPoint.y() - ibtm;

        // Set larger of horizontal/vertical distance to zero.
        // A valid distance is NEVER larger than world's side's length!
        uint32_t d_lft = _lft & -(_lft <  _rgt) & -(_lft < worldDimension.getWidth());
        uint32_t d_rgt = _rgt & -(_rgt <= _lft) & -(_rgt < worldDimension.getWidth());
        uint32_t d_top = _top & -(_top <  _btm) & -(_top < worldDimension.getHeight());
        uint32_t d_btm = _btm & -(_btm <= _top) & -(_btm < worldDimension.getHeight());

        // Make sure plate doesn't grow bigger than the system it's in!
        if (bounds->width() + d_lft + d_rgt > worldDimension.getWidth())
        {
            d_lft = 0;
            d_rgt = worldDimension.getWidth() - bounds->width();
        }

        if (bounds->height() + d_top + d_btm > worldDimension.getHeight())
        {
            d_top = 0;
            d_btm = worldDimension.getHeight() - bounds->height();
        }

        // Index out of bounds, but nowhere to grow!
        ASSERT(d_lft + d_rgt + d_top + d_btm != 0, "Invalid plate growth deltas");

        const auto old_width  = bounds->width();
        const auto old_height = bounds->height();

        bounds->shift(Platec::vec2f(-1.0*d_lft, -1.0*d_top));
        bounds->grow(Platec::vec2ui(d_lft + d_rgt, d_top + d_btm));

        auto tmph = HeightMap(bounds->getDimension());
        auto    tmpa = AgeMap(bounds->getDimension());
        auto tmps = std::vector<uint32_t>(bounds->area());

        // copy old plate into new.
        for (uint32_t j = 0; j < old_height; ++j)
        {
            const uint32_t dest_i = (d_top + j) * bounds->width() + d_lft;
            const uint32_t src_i = j * old_width;
            std::copy(map.getData().begin() + src_i,map.getData().begin() + src_i +old_width,
                       tmph.getData().begin()+dest_i );
            std::copy(age_map.getData().begin() + src_i,age_map.getData().begin() + src_i +old_width,
                       tmpa.getData().begin()+dest_i );
            std::copy(segments->getSegment().begin() + src_i,segments->getSegment().begin() + src_i +old_width,
                       tmps.begin()+dest_i );
        }

        map     = tmph;
        age_map = tmpa;
        segments->reassign(bounds->area(),tmps);

        // Shift all segment data to match new coordinates.
        segments->shift(Platec::vec2ui(d_lft, d_top));


        index = bounds->getValidMapIndex(normPoint);

    }

    // Update crust's age.
    // If old crust exists, new age is mean of original and supplied ages.
    // If no new crust is added, original time remains intact.    
    if(z > 0)
    {
        if(map[index.first] != 0)
        {
            t = (uint32_t)((map[index.first] * age_map[index.first] + z * t) /
                                       (map[index.first] + z));
        }
        age_map[index.first] = t ;
    }
    


    mass.incMass(-1.0f * map[index.first]);
    mass.incMass(z);      // Update mass counter.
    map[index.first] = z;     // Set new crust height to desired location.
}

ContinentId plate::selectCollisionSegment(const Platec::vec2ui& point) const
{
    return segments->id(bounds->getValidMapIndex(point).first);
}

///////////////////////////////////////////////////////////////////////////////
/// Private methods ///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

uint32_t plate::createSegment(const Platec::vec2ui& point)
{
    return mySegmentCreator->createSegment(point,worldDimension);
}

ISegmentData& plate::getContinentAt(const Platec::vec2ui& point)
{
    return segments->getSegmentData(segments->getContinentAt(point,worldDimension));
}

const ISegmentData& plate::getContinentAt(const Platec::vec2ui& point) const
{
    return segments->getSegmentData(segments->getContinentAt(point,worldDimension));
}
