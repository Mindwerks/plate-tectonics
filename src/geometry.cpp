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

//
// IntPoint
//

IntPoint::IntPoint(int x, int y)
    : _x(x), _y(y)
{ }

int IntPoint::getX() const
{
    return _x;
}

int IntPoint::getY() const
{
    return _y;
}

//
// FloatPoint
//

FloatPoint::FloatPoint(float x, float y)
    : _x(x), _y(y)
{ }

float FloatPoint::getX() const
{
    return _x;
}

float FloatPoint::getY() const
{
    return _y;
}

void FloatPoint::shift(float dx, float dy, const WorldDimension& _worldDimension)
{
    const uint32_t world_width = _worldDimension.getWidth();
    _x += dx;
    _x += _x > 0 ? 0 : world_width;
    _x -= _x < world_width ? 0 : world_width;

    const uint32_t world_height = _worldDimension.getHeight();
    _y += dy;
    _y += _y > 0 ? 0 : world_height;
    _y -= _y < world_height ? 0 : world_height;

    ASSERT(_worldDimension.contains(*this), "Point not in world!");
}

//
// Dimension
//

Dimension::Dimension(uint32_t width, uint32_t height) :
    _width(width), _height(height)
{
}

Dimension::Dimension(const Dimension& original) :
    _width(original.getWidth()), _height(original.getHeight())
{
}

bool Dimension::contains(const uint32_t x, const uint32_t y) const
{
    return (x >= 0 && x < _width && y >= 0 && y < _height);
}

bool Dimension::contains(const float x, const float y) const
{
    return (x >= 0 && x < _width && y >= 0 && y < _height);
}

bool Dimension::contains(const FloatPoint& p) const
{
    return (p.getX() >= 0 && p.getX() < _width && p.getY() >= 0 && p.getY() < _height);
}

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

WorldDimension::WorldDimension(const WorldDimension& original) : Dimension(original)
{
};

uint32_t WorldDimension::getMax() const
{
    return _width > _height ? _width : _height;
}

uint32_t WorldDimension::xMod(uint32_t x) const
{
    return (x + _width) % _width;
}

uint32_t WorldDimension::yMod(uint32_t y) const
{
    return (y + _height) % _height;
}

void WorldDimension::normalize(uint32_t& x, uint32_t& y) const
{
    x %= _width;
    y %= _height;
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

uint32_t WorldDimension::largerSize() const
{
    return _width > _height ? _width : _height;
}
