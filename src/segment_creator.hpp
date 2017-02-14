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

#include <vector>
#include "utils.hpp"
#include "geometry.hpp"
#include "heightmap.hpp"

typedef uint32_t ContinentId;

class IBounds;
class ISegments;

class ISegmentCreator
{
public:
    virtual ContinentId createSegment(uint32_t wx, uint32_t wy) const = 0;
};

class MySegmentCreator : public ISegmentCreator
{
public:
    MySegmentCreator(IBounds& bounds, ISegments* segments, HeightMap& map_,
                     const WorldDimension& worldDimension)
        : _bounds(bounds), _segments(segments), map(map_),
          _worldDimension(worldDimension)
    {

    }
    /// Separate a continent at (X, Y) to its own partition.
    ///
    /// Method analyzes the pixels 4-ways adjacent at the given location
    /// and labels all connected continental points with same segment ID.
    ///
    /// @param	x	Offset on the local height map along X axis.
    /// @param	y	Offset on the local height map along Y axis.
    /// @return	ID of created segment on success, otherwise -1.
    ContinentId createSegment(uint32_t wx, uint32_t wy) const throw();
private:
    uint32_t calcDirection(uint32_t x, uint32_t y, const uint32_t origin_index, const uint32_t ID) const;
    void scanSpans(const uint32_t line, uint32_t& start, uint32_t& end,
                   std::vector<uint32_t>* spans_todo, std::vector<uint32_t>* spans_done) const;
    const WorldDimension _worldDimension;
    IBounds& _bounds;
    ISegments* _segments;
    HeightMap& map;
};

#endif
