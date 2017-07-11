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

#ifndef PLATE_HPP
#define PLATE_HPP

#define NOMINMAX

#include <algorithm>

#include <vector>
#include <memory>
#include <cmath>     // sin, cos
#include <cstring>
#include "simplerandom.hpp"
#include "heightmap.hpp"
#include "segment_data.hpp"
#include "utils.hpp"
#include "bounds.hpp"
#include "movement.hpp"
#include "mass.hpp"
#include "segments.hpp"

class IPlate : public IMass, public IMovement
{
public:
};


struct surroundingPoints
{
public:
    float_t westCrust = 0.f, eastCrust= 0.f, northCrust= 0.f, southCrust= 0.f;
    uint32_t westIndex = 0, eastIndex= 0,  northIndex= 0, southIndex= 0, centerIndex=0;
    
    surroundingPoints(){};
    
    bool centerIsLowest() const
    {
        return (westCrust +eastCrust+northCrust+eastCrust) == 0;
    }
    
    bool oneIsLower() const
    {
        return (westCrust *eastCrust*northCrust*eastCrust) == 0;
    }
    
    uint32_t getLowestIndex()
    {
        if(centerIsLowest())
        {
            return centerIndex;
        }
        float lowest_crust = std::numeric_limits<float_t>::max();
        uint32_t dest = 0;
        
       if (westCrust < lowest_crust && westCrust != 0.f) {
            lowest_crust = westCrust;
            dest = westIndex;
        }

        if (eastCrust < lowest_crust && eastCrust != 0.f) {
            lowest_crust = eastCrust;
            dest = eastIndex;
        }

        if (northCrust < lowest_crust && northCrust != 0.f) {
            lowest_crust = northCrust;
            dest = northIndex;
        }

        if (southCrust < lowest_crust && southCrust != 0.f) {
            lowest_crust = southCrust;
            dest = southIndex;
        }
        
        return dest;

    }

};


class plate : public IPlate
{
private:
    const Dimension worldDimension;
    SimpleRandom randsource;
    HeightMap map;        ///< Bitmap of plate's structure/height.
    AgeMap age_map;       ///< Bitmap of plate's soil's age: timestamp of creation.
    std::shared_ptr<Bounds> bounds;
    Mass mass;
    Movement movement;
    std::shared_ptr<Segments> segments;
    std::shared_ptr<MySegmentCreator> mySegmentCreator;    
    
public:

    /// Initializes plate with the supplied height map.
    ///
    /// @param  m              Pointer to array to height map of terrain.
    /// @param  w              Width of height map in pixels.
    /// @param  h              Height of height map in pixels.
    /// @param  _x             X of height map's left-top corner on world map.
    /// @param  _y             Y of height map's left-top corner on world map.
    /// @param  worldDimension Dimension of world map's either side in pixels.
    plate(const Dimension& worldDimension_, const long seed,const HeightMap&  m, 
            const Dimension& plateDimension,
            const Platec::vec2f& topLeftCorner,
         const uint32_t plate_age) ;


    /// Increment collision counter of the continent at given location.
    ///
    /// @param  wx  X coordinate of collision point on world map.
    /// @param  wy  Y coordinate of collision point on world map.
    /// @return Surface area of the collided continent (HACK!)
    uint32_t addCollision(const Platec::vec2ui& point);

    /// Add crust to plate as result of continental collision.
    ///
    /// @param  x   Location of new crust on global world map (X).
    /// @param  y   Location of new crust on global world map (Y).
    /// @param  z   Amount of crust to add.
    /// @param  t   Time of creation of new crust.
    /// @param activeContinent Segment ID of the continent that's processed.
    void addCrustByCollision(const Platec::vec2ui& point,const float_t z,
                            const uint32_t time,const ContinentId activeContinent);

    /// Simulates subduction of oceanic plate under this plate.
    ///
    /// Subduction is simulated by calculating the distance on surface
    /// that subducting sediment will travel under the plate until the
    /// subducting slab has reached certain depth where the heat triggers
    /// the melting and uprising of molten magma.
    ///
    /// @param  x   Origin of subduction on global world map (X).
    /// @param  y   Origin of subduction on global world map (Y).
    /// @param  z   Amount of sediment that subducts.
    /// @param  t   Time of creation of new crust.
    /// @param  dx  Direction of the subducting plate (X).
    /// @param  dy  Direction of the subducting plate (Y).
    void addCrustBySubduction(const Platec::vec2ui& originPoint,const float_t sediment,const uint32_t time,
                              const Platec::vec2f& dir);

    /// Add continental crust from this plate as part of other plate.
    ///
    /// Aggregation of two continents is the event where the collided
    /// pieces of crust fuse together at the point of collision. It is
    /// crucial to merge not only the collided pieces of crust but also
    /// the entire continent that's part of the colliding tad of crust
    /// However, because one plate can contain many islands and pieces of
    /// continents, the merging must be done WITHOUT merging the entire
    /// plate and all those continental pieces that have NOTHING to do with
    /// the collision in question.
    ///
    /// @param  p   Pointer to the receiving plate.
    /// @param  wx  X coordinate of collision point on world map.
    /// @param  wy  Y coordinate of collision point on world map.
    /// @return Amount of crust aggregated to destination plate.
    float aggregateCrust(plate& p, Platec::vec2ui point);

    /// Decrease the speed of plate amount relative to its total mass.
    ///
    /// Method decreses the speed of plate due to friction that occurs when
    /// two plates collide. The amount of reduction depends of the amount
    /// of mass that causes friction (i.e. that has collided) compared to
    /// the total mass of the plate. Thus big chunk of crust colliding to
    /// a small plate will halt it but have little effect on a huge plate.
    ///
    /// @param  deforming_mass Amount of mass deformed in collision.
    void applyFriction(const float_t deforming_mass);

    /// Method collides two plates according to Newton's laws of motion.
    ///
    /// The velocity and direction of both plates are updated using
    /// impulse forces following the collision according to Newton's laws
    /// of motion. Deformations are not applied but energy consumed by the
    /// deformation process is taken away from plate's momentum.
    ///
    /// @param  p   Plate to test against.
    /// @param  wx  X coordinate of collision point on world map.
    /// @param  wy  Y coordinate of collision point on world map.
    /// @param  coll_mass Amount of colliding mass from source plate.
    void collide(plate& p, const float_t coll_mass);

    /// Apply plate wide erosion algorithm.
    ///
    /// Plates total mass and the center of mass are updated.
    ///
    /// @param  lower_bound Sets limit below which there's no erosion.
    void erode(float lower_bound);

    /// Retrieve collision statistics of continent at given location.
    ///
    /// @param  wx  X coordinate of collision point on world map.
    /// @param  wy  Y coordinate of collision point on world map.
    /// @param[in, out] count Destination for the count of collisions.
    /// @param[in, out] count Destination for the % of area that collided.
    const std::pair<uint32_t,float_t> getCollisionInfo(const Platec::vec2ui& point) const;

    /// Retrieve the surface area of continent lying at desired location.
    ///
    /// @param  wx  X coordinate of collision point on world map.
    /// @param  wy  Y coordinate of collision point on world map.
    /// @return Area of continent at desired location or 0 if none.
    uint32_t getContinentArea(const Platec::vec2ui& point) const;

    /// Get the amount of plate's crustal material at some location.
    ///
    /// @param  x   Offset on the global world map along X axis.
    /// @param  y   Offset on the global world map along Y axis.
    /// @return     Amount of crust at requested location.
    float_t getCrust(const Platec::vec2ui& point) const;

    /// Get the timestamp of plate's crustal material at some location.
    ///
    /// @param  x   Offset on the global world map along X axis.
    /// @param  y   Offset on the global world map along Y axis.
    /// @return     Timestamp of creation of crust at the location.
    ///                     Zero is returned if location contains no crust.
    uint32_t getCrustTimestamp(const Platec::vec2ui& point) const;

   
    AgeMap& getAgeMap();
    
    HeightMap& getHeigthMap();

    void move();///< Moves plate along it's trajectory.

    /// Clear any earlier continental crust partitions.
    ///
    /// Plate has an internal bookkeeping of distinct areas of continental
    /// crust for more realistic collision responce. However as the number
    /// of collisions that plate experiences grows, so does the bookkeeping
    /// of a continent become more and more inaccurate. Finally it results
    /// in striking artefacts that cannot overlooked.
    ///
    /// To alleviate this problem without the need of per iteration
    /// recalculations plate supplies caller a method to reset its
    /// bookkeeping and start clean.
    void resetSegments();

    /// Remember the currently processed continent's segment number.
    ///
    /// @param  coll_x  Origin of collision on global world map (X).
    /// @param  coll_y  Origin of collision on global world map (Y).
    /// @return the Id of the continent being processed
    ContinentId selectCollisionSegment(const Platec::vec2ui& point) const ;

    /// Set the amount of plate's crustal material at some location.
    ///
    /// If amount of crust to be set is negative, it'll be set to zero.
    ///
    /// @param  x   Offset on the global world map along X axis.
    /// @param  y   Offset on the global world map along Y axis.
    /// @param  z   Amount of crust at given location.
    /// @param  t   Time of creation of new crust.
    void setCrust(const Platec::vec2ui& point, float_t z, uint32_t t);

    float getMass() const {
        return mass.getMass();
    }
    float getMomentum() const  {
        return movement.momentum(mass);
    }
    uint32_t getHeight() const  {
        return bounds->height();
    }
    uint32_t  getLeftAsUint() const  {
        return bounds->left();
    }
    uint32_t  getTopAsUint() const  {
        return bounds->top();
    }
    float_t getVelocity() const {
        return movement.getVelocity();
    }

    Platec::vec2f velocityUnitVector() const {
        return movement.velocityUnitVector();
    }

    const Platec::vec2f getVelocityVector() const {
        return movement.velocityVector();
    }


    uint32_t getWidth() const  {
        return bounds->width();
    }
    bool   isEmpty() const  {
        return mass.isNull();
    }

    const Platec::vec2f massCenter() const {
        return mass.massCenter();
    }

    void decImpulse(const Platec::vec2f& delta) {
        movement.decImpulse(delta);
    }

    // visible for testing
    const surroundingPoints calculateCrust(const uint32_t index) const;

    // Visible for testing
    void injectSegments( std::shared_ptr<ISegments> newSegment)
    {
        this->segments = std::static_pointer_cast<Segments>(newSegment);
    }
private:

    ISegmentData& getContinentAt(const Platec::vec2ui& point);
    const ISegmentData& getContinentAt(const Platec::vec2ui& point) const;
    std::vector<surroundingPoints> findRiverSources(const float_t lower_bound);
    std::vector<uint32_t> flowRivers(std::vector<surroundingPoints> sources,std::vector<uint32_t> foundIndices = std::vector<uint32_t>());
    uint32_t createSegment(const Platec::vec2ui& point);


};

#endif
