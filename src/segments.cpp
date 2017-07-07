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

#include <cstring>
#include "segments.hpp"


Segments::Segments(uint32_t plate_area) : area(plate_area),
        segment(std::vector<uint32_t>(plate_area,255))
{
}

const uint32_t Segments::getArea() const
{
    return area;
}

void Segments::reset()
{
    seg_data.clear();
    std::vector<uint32_t>(area,-1).swap(segment);
}

void Segments::reassign(const uint32_t newarea,const std::vector<uint32_t>& tmps)
{
    area = newarea;
    segment = std::move(tmps);
}

void Segments::shift(const Platec::vec2ui& dir)
{
    for (auto& data : seg_data)
    {
        data.shift(dir);
    }
}

const uint32_t Segments::size() const
{
    return seg_data.size();
}

const ISegmentData& Segments::getSegmentData(uint32_t index) const
{
    return seg_data.at(index);
}

ISegmentData& Segments::getSegmentData(uint32_t index)
{
    return seg_data.at(index);
}

void Segments::add(const SegmentData& data) {
    seg_data.emplace_back(data);
}

ContinentId Segments::getContinentAt(const Platec::vec2ui& point,
                                       const Dimension& worldDimension) const
{

    auto index = bounds->getValidMapIndex(point);
    ContinentId seg = id(index.first);

    if (seg >= size()) {
        // in this case, we consider as const this call because we calculate
        // something that we would calculate anyway, so the segments are
        // a sort of cache
        //seg = const_cast<plate*>(this)->createSegment(lx, ly);
        seg = segmentCreator->createSegment(index.second,worldDimension);
    }

    ASSERT(seg < size(), "Could not create segment");
    return seg;
}

const std::vector<ContinentId>& Segments::getSegment() const {
    return segment;
}

const ContinentId& Segments::id(const uint32_t index) const {
    return segment[index];
}

ContinentId& Segments::id(const uint32_t index) {
    return segment.at(index);
}

void Segments::setBounds(const std::shared_ptr<Bounds>& bounds) {
    this->bounds = bounds;
}

void Segments::setId(const uint32_t index, const ContinentId id) {
    segment.at(index) = id;
}

void Segments::setSegmentCreator(const std::shared_ptr<ISegmentCreator>& segmentCreator) {
    this->segmentCreator = segmentCreator;
}

