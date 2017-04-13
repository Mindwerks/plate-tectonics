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

#include "geometry.hpp"

Dimension::Dimension(uint32_t width, uint32_t height) :
    width(width), height(height)
{
}



void Dimension::grow(uint32_t amountX, uint32_t amountY)
{
    width += amountX;
    height += amountY;
}

//
// WorldDimension
//

WorldDimension::WorldDimension(uint32_t width, uint32_t height) : Dimension(width, height)
{
};

uint32_t WorldDimension::getMax() const
{
    return std::max(width, height);
}

uint32_t WorldDimension::xMod(const uint32_t x) const
{
    return x>= width ? x-width : x;
}

uint32_t WorldDimension::yMod(const uint32_t y) const
{
   return y>= height ? y-height : y;
}

Platec::Point2D<uint32_t>  WorldDimension::normalize(const Platec::Point2D<uint32_t>& point) const
{
   return Platec::Point2D<uint32_t>( (point.x() >= width) ? point.x()-width : point.x(),
                                      (point.y() >= height) ? point.y() -height : point.y() );
}

uint32_t WorldDimension::indexOf(const uint32_t x, const uint32_t y) const
{
    return y * getWidth() + x;
}

uint32_t WorldDimension::indexOf(const Platec::Point2D<uint32_t>& point) const 
{
    return point.y() * getWidth() + point.x();
}



uint32_t WorldDimension::lineIndex(const uint32_t y) const
{
    ASSERT(y < height, "y is not valid");
    return indexOf(0, y);
}

uint32_t WorldDimension::yFromIndex(const uint32_t index) const
{
    return index / width;
}

uint32_t WorldDimension::xFromIndex(const uint32_t index) const
{
    return index - yFromIndex(index) * width;
}

uint32_t WorldDimension::normalizedIndexOf(const uint32_t x, const uint32_t y) const
{
    return indexOf(xMod(x), yMod(y));
}

uint32_t WorldDimension::xCap(const uint32_t x) const
{
    return std::min(x,width-1);
}

uint32_t WorldDimension::yCap(const uint32_t y) const
{
    return std::min(y,height-1);
}

Platec::Point2D<uint32_t> WorldDimension::xMod(const Platec::Point2D<uint32_t>& point) const
{
    if(point.x()>= width)
    {
        return Platec::Point2D<uint32_t>(point.x()-width,point.y());
    }
    return point;
}

Platec::Point2D<uint32_t> WorldDimension::yMod(const Platec::Point2D<uint32_t>& point) const
{
    if(point.y()>= height)
    {
        return Platec::Point2D<uint32_t>(point.x(), point.y()-height);
    }
    return point;
}

Platec::Point2D<uint32_t> WorldDimension::pointMod(const Platec::Point2D<uint32_t>& point) const 
{
    return Platec::Point2D<uint32_t>(point.x()>= width ? point.x()-width: point.x(),
                                    point.y()>= height ? point.y()-height : point.y());
}


uint32_t WorldDimension::normalizedIndexOf(const Platec::Point2D<uint32_t>& point) const 
{
    return indexOf(pointMod(point));
}

Platec::Point2D<uint32_t> WorldDimension::xCap(const Platec::Point2D<uint32_t>& point) const
{
    return Platec::Point2D<uint32_t> (std::min(point.x(),width-1),point.y());
}

Platec::Point2D<uint32_t> WorldDimension::yCap(const Platec::Point2D<uint32_t>& point) const
{
    return Platec::Point2D<uint32_t> (point.x(),std::min(point.y(),height-1));
}


