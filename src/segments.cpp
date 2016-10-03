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

#include "segments.hpp"

Segments::Segments(uint32_t plate_area)
{
    _area = plate_area;
    segment = new uint32_t[plate_area];
    memset(segment, 255, plate_area * sizeof(uint32_t));
}

Segments::~Segments()
{
    delete[] segment;
    segment = NULL;
    _area = 0;
    for (int i = 0; i < seg_data.size(); i++) {
        delete seg_data[i];
    }
}

uint32_t Segments::area()
{
    return _area;
}

void Segments::reset()
{
    memset(segment, -1, sizeof(uint32_t) * _area);
    for (int i = 0; i < seg_data.size(); i++) {
        delete seg_data[i];
    }
    seg_data.clear();
}

void Segments::reassign(uint32_t newarea, uint32_t* tmps)
{
    delete[] segment;
    _area = newarea;
    segment = tmps;
}

void Segments::shift(uint32_t d_lft, uint32_t d_top)
{
    for (uint32_t s = 0; s < seg_data.size(); ++s)
    {
        seg_data[s]->shift(d_lft, d_top);
    }
}

uint32_t Segments::size() const
{
    return (uint32_t)seg_data.size();
}

const ISegmentData& Segments::operator[](uint32_t index) const
{
    ASSERT(index < seg_data.size(), "Invalid index");
    return *seg_data[index];
}

ISegmentData& Segments::operator[](uint32_t index)
{
    ASSERT(index < seg_data.size(), "Invalid index");
    return *seg_data[index];
}

void Segments::add(ISegmentData* data) {
    seg_data.push_back(data);
}

ContinentId Segments::getContinentAt(int x, int y) const
{
    ASSERT(_bounds, "Bounds not set");
    ASSERT(_segmentCreator, "SegmentCreator not set");
    uint32_t lx = x, ly = y;
    uint32_t index = _bounds->getValidMapIndex(&lx, &ly);
    ContinentId seg = id(index);

    if (seg >= size()) {
        // in this case, we consider as const this call because we calculate
        // something that we would calculate anyway, so the segments are
        // a sort of cache
        //seg = const_cast<plate*>(this)->createSegment(lx, ly);
        seg = _segmentCreator->createSegment(lx, ly);
    }

    ASSERT(seg < size(), "Could not create segment");
    return seg;
}
