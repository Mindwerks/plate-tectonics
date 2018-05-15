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

#ifndef SEGMENTS_HPP
#define SEGMENTS_HPP

#define NOMINMAX

#include <vector>
#include <cmath>     // sin, cos
#include "simplerandom.hpp"
#include "heightmap.hpp"
#include "segment_data.hpp"
#include "utils.hpp"
#include "bounds.hpp"
#include "movement.hpp"
#include "mass.hpp"
#include "segment_creator.hpp"

typedef uint32_t ContinentId;

class ISegments
{
public:
    virtual  uint32_t getArea() const = 0;
    virtual void reset() = 0;
    virtual void reassign(const uint32_t newarea,const std::vector<uint32_t>& tmps) = 0;
    virtual void shift(const Platec::vec2ui& dir) = 0;
    virtual  uint32_t size() const = 0;
    virtual const ISegmentData& getSegmentData(const uint32_t index) const = 0;
    virtual ISegmentData& getSegmentData(const uint32_t index) = 0;
    virtual void add(const SegmentData& data) = 0;
    // Continent at the give world index
    virtual const ContinentId& id(const uint32_t index) const = 0;
    // Continent at the give world index
    virtual ContinentId& id(const uint32_t index) = 0;
    virtual void setId(const uint32_t index,const ContinentId id) = 0;
    virtual ContinentId getContinentAt(const Platec::vec2ui& point,
                                       const Dimension& worldDimension ) const = 0;
};

class Segments : public ISegments
{
private:
    std::vector<SegmentData> seg_data; ///< Details of each crust segment.
    std::vector<ContinentId> segment;              ///< Segment ID of each piece of continental crust.
    uint32_t area; /// Should be the same as the bounds area of the plate
    std::shared_ptr<ISegmentCreator> segmentCreator;
    std::shared_ptr<Bounds> bounds;    
    
public:
    Segments(uint32_t plate_area);
    void setSegmentCreator(const std::shared_ptr<ISegmentCreator>& segmentCreator_);
    void setBounds(const std::shared_ptr<Bounds>&  bounds_);
    uint32_t getArea() const override;
    void reset() override;
    void reassign(const uint32_t newarea,const std::vector<uint32_t>& tmps) override;
    void shift(const Platec::vec2ui& dir) override;
     uint32_t size() const override;
    const ISegmentData& getSegmentData(const uint32_t index) const override; 
    ISegmentData& getSegmentData(const uint32_t index) override;
    void add(const SegmentData& data) override;
    const ContinentId& id(const uint32_t index) const override;
    ContinentId& id(const uint32_t index) override;
    void setId(const uint32_t index,const ContinentId id) override;
    ContinentId getContinentAt(const Platec::vec2ui& point,
                              const Dimension& worldDimension) const override;
    const std::vector<ContinentId>& getSegment() const;
};

#endif
