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

#ifndef SEGMENT_CREATOR_HPP
#define SEGMENT_CREATOR_HPP

#define NOMINMAX 

#include <memory>
#include <vector>
#include "utils.hpp"
#include "dimension.h"
#include "heightmap.hpp"

static constexpr float CONT_BASE = 1.0; ///< Height limit that separates seas from dry land.

typedef uint32_t ContinentId;

class Bounds;
class ISegments;

class ISegmentCreator
{
public:
    virtual ContinentId createSegment(const Platec::vec2ui& point, 
                                    const Dimension& worldDimension) = 0;
};

class Span
{
public:
    uint32_t start;
    uint32_t end;    
    Span():start(0),end(0){}
    Span(const uint32_t val):start(val),end(val){}
    Span(const uint32_t start_, const uint32_t end_):start(start_),end(end_){}
    
    bool inside(const uint32_t val) const
    {
        return val >= start &&  val <= end;
    }
    
    bool notValid()
    {
       return start > end;
    }
};

class MySegmentCreator : public ISegmentCreator
{
public:
    MySegmentCreator(std::shared_ptr<Bounds> bounds_, std::shared_ptr<ISegments> segments_, HeightMap& map_);
    /// Separate a continent at (X, Y) to its own partition.
    ///
    /// Method analyzes the pixels 4-ways adjacent at the given location
    /// and labels all connected continental points with same segment ID.
    ///
    /// @param	x	Offset on the local height map along X axis.
    /// @param	y	Offset on the local height map along Y axis.
    /// @return	ID of created segment on success, otherwise -1.
    ContinentId createSegment(const Platec::vec2ui& point, 
                                    const Dimension& worldDimension);
private:
    uint32_t calcDirection(const Platec::vec2ui& point, const uint32_t origin_index, const uint32_t ID) const;
    Span scanSpans( std::vector<Span>& spans_todo, std::vector<Span>& spans_done) const;
    
     uint32_t getLeftIndex(const int32_t originIndex) const;
     uint32_t getRightIndex(const int32_t originIndex) const;
     uint32_t getTopIndex(const int32_t originIndex) const;
     uint32_t getBottomIndex(const int32_t originIndex) const;

     bool hasLowerID(const uint32_t index, const ContinentId ID) const;
     bool usablePoint(const uint32_t index, const ContinentId ID) const;
    std::vector<Span> fillLineWithID(const Span& span, const uint32_t line,
                                        const ContinentId ID ) ;
    std::shared_ptr<Bounds>  bounds;
    std::shared_ptr<ISegments> segments;
    HeightMap& map;

};

#endif
