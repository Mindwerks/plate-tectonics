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

//template<class T>
//void Point2D::shift(float_t dx, float_t dy, const WorldDimension& _worldDimension) {
//        const uint32_t world_width = _worldDimension.getWidth();
//        x_value += dx;
//        x_value += x_value > 0 ? 0 : world_width;
//        x_value -= x_value < world_width ? 0 : world_width;
//
//        const uint32_t world_height = _worldDimension.getHeight();
//        y_value += dy;
//        y_value += y_value > 0 ? 0 : world_height;
//        y_value -= y_value < world_height ? 0 : world_height;
//}


//
// Dimension
//

Dimension::Dimension(uint32_t width, uint32_t height) :
    _width(width), _height(height)
{
}



//bool Dimension::contains(const Platec::Point2D<T>& p) const
//{
//    return (p.x() >= 0.0f && p.x() < _width && p.y() >= 0.0f && p.y() < _height);
//}

void Dimension::grow(uint32_t amountX, uint32_t amountY)
{
    _width += amountX;
    _height += amountY;
}

//
// WorldDimension
//

WorldDimension::WorldDimension(uint32_t width, uint32_t height) : Dimension(width, height)
{
};

uint32_t WorldDimension::getMax() const
{
    return std::max(_width, _height);
}

uint32_t WorldDimension::xMod(uint32_t x) const
{
    return x>= _width ? x-_width : x;
}

uint32_t WorldDimension::yMod(uint32_t y) const
{
   return y>= _height ? y-_height : y;
}

void WorldDimension::normalize(uint32_t& x, uint32_t& y) const
{
    x = (x>= _width) ? x-_width : x;
    y = (y>= _height) ? y-_height : y;
}

uint32_t WorldDimension::indexOf(const uint32_t x, const uint32_t y) const
{
    return y * getWidth() + x;
}

uint32_t WorldDimension::lineIndex(const uint32_t y) const
{
    ASSERT(y >= 0 && y < _height, "y is not valid");
    return indexOf(0, y);
}

uint32_t WorldDimension::yFromIndex(const uint32_t index) const
{
    return index / _width;
}

uint32_t WorldDimension::xFromIndex(const uint32_t index) const
{
    const uint32_t y = yFromIndex(index);
    return index - y * _width;
}

uint32_t WorldDimension::normalizedIndexOf(const uint32_t x, const uint32_t y) const
{
    return indexOf(xMod(x), yMod(y));
}

uint32_t WorldDimension::xCap(const uint32_t x) const
{
    return x < _width ? x : (_width-1);
}

uint32_t WorldDimension::yCap(const uint32_t y) const
{
    return y < _height ? y : (_height-1);
}