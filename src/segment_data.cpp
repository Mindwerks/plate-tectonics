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

#include "segment_data.hpp"

SegmentData::SegmentData(const Platec::vec2ui& pointLeftTop,
                        const Platec::vec2ui& pointRightBottom,
                         uint32_t area) 
                    : pointLeftTop(pointLeftTop), 
                      pointRightBottom(pointRightBottom),      
                     area(area), coll_count(0) {};

void SegmentData::enlarge_to_contain(const Platec::vec2ui& point)
{
    if (point.y() < getTop()) {
        pointLeftTop = Platec::vec2ui(getLeft(), point.y());
    } else if (point.y() > getBottom()) {
        pointRightBottom = Platec::vec2ui(getRight(), point.y());
    }
    if (point.x() < getLeft()) {
        pointLeftTop = Platec::vec2ui( point.x(),getTop());
    } else if (point.x() > getRight()) {
         pointRightBottom = Platec::vec2ui( point.x(),getBottom());
    }
};

uint32_t SegmentData::getLeft() const
{
    return pointLeftTop.x();
};

uint32_t SegmentData::getRight() const
{
    return pointRightBottom.x();
};

uint32_t SegmentData::getTop() const
{
    return pointLeftTop.y();
};

uint32_t SegmentData::getBottom() const
{
    return pointRightBottom.y();
};

void SegmentData::shift(const Platec::vec2ui& shiftDir)
{
    pointRightBottom.shift(shiftDir);
    pointLeftTop.shift(shiftDir);
};

void SegmentData::setLeft(const uint32_t v)
{
    pointLeftTop = Platec::vec2ui(v, getTop());
};

void SegmentData::setRight(const uint32_t v)
{
    pointRightBottom = Platec::vec2ui(v, getBottom());
};

void SegmentData::setTop(const uint32_t v)
{
    pointLeftTop = Platec::vec2ui(getLeft(),v);
};

void SegmentData::setBottom(const uint32_t v)
{
    pointRightBottom = Platec::vec2ui(getRight(),v);
};

bool SegmentData::isEmpty() const
{
    return area == 0;
};

void SegmentData::incCollCount()
{
    ++coll_count;
};

void SegmentData::incArea()
{
    ++area;
};

void SegmentData::incArea(const uint32_t amount)
{
    area += amount;
};

uint32_t SegmentData::getArea() const
{
    return area;
};

uint32_t SegmentData::collCount() const
{
    return coll_count;
}

void SegmentData::markNonExistent()
{
    area = 0;
}
