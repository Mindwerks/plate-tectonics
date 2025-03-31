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

#include <vector>
#include <cmath>     // sin, cos
#include "simplerandom.hpp"
#include "heightmap.hpp"
#include "rectangle.hpp"
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
    virtual ~ISegments() {}
    virtual uint32_t area() = 0;
    virtual void reset() = 0;
    virtual void reassign(uint32_t newarea, uint32_t* tmps) = 0;
    virtual void shift(uint32_t d_lft, uint32_t d_top) = 0;
    virtual uint32_t size() const = 0;
    virtual const ISegmentData& operator[](uint32_t index) const = 0;
    virtual ISegmentData& operator[](uint32_t index) = 0;
    virtual void add(ISegmentData* data) = 0;
    // Continent at the give world index
    virtual const ContinentId& id(uint32_t index) const = 0;
    // Continent at the give world index
    virtual ContinentId& id(uint32_t index) = 0;
    virtual void setId(uint32_t index, ContinentId id) = 0;
    virtual ContinentId getContinentAt(int x, int y) const = 0;
};

class Segments : public ISegments
{
public:
    Segments(uint32_t plate_area);
    ~Segments();
    void setSegmentCreator(ISegmentCreator* segmentCreator)
    {
        _segmentCreator = segmentCreator;
    }
    void setBounds(IBounds* bounds)
    {
        _bounds = bounds;
    }
    uint32_t area();
    void reset();
    void reassign(uint32_t newarea, uint32_t* tmps);
    void shift(uint32_t d_lft, uint32_t d_top);
    uint32_t size() const;
    const ISegmentData& operator[](uint32_t index) const;
    ISegmentData& operator[](uint32_t index);
    void add(ISegmentData* data);
    const ContinentId& id(uint32_t index) const {
        return segment[index];
    }
    ContinentId& id(uint32_t index) {
        return segment[index];
    }
    void setId(uint32_t index, ContinentId id) {
        segment[index] = id;
    }
    ContinentId getContinentAt(int x, int y) const;
private:
    std::vector<ISegmentData*> seg_data; ///< Details of each crust segment.
    ContinentId* segment;              ///< Segment ID of each piece of continental crust.
    int _area; /// Should be the same as the bounds area of the plate
    ISegmentCreator* _segmentCreator;
    IBounds* _bounds;
};

#endif
