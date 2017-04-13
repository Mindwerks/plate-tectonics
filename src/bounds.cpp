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

#include "bounds.hpp"

Bounds::Bounds(const WorldDimension& worldDimension, const Platec::Point2D<float_t>& position,
               const Dimension& dimension)
    : _worldDimension(worldDimension),
      _position(position),
      _dimension(dimension) {
    ASSERT(_dimension.getWidth() <= _worldDimension.getWidth() &&
           _dimension.getHeight() <= _worldDimension.getHeight(),
           "Bounds are larger than the world containing it");
}

uint32_t Bounds::index(uint32_t x, uint32_t y) const {
    ASSERT(x < _dimension.getWidth() && y < _dimension.getHeight(),
           "Invalid coordinates");
    return y * _dimension.getWidth() + x;
}

uint32_t Bounds::area() const {
    return _dimension.getArea();
}

uint32_t Bounds::width() const {
    return _dimension.getWidth();
}

uint32_t Bounds::height() const {
    return _dimension.getHeight();
}

uint32_t Bounds::leftAsUint() const {
    return (uint32_t)_position.x();
}

uint32_t Bounds::topAsUint() const {
    return (uint32_t)_position.y();
}

uint32_t Bounds::rightAsUintNonInclusive() const {
    return leftAsUint() + width() - 1;
}

uint32_t Bounds::bottomAsUintNonInclusive() const {
    return topAsUint() + height() - 1;
}

bool Bounds::containsWorldPoint(uint32_t x, uint32_t y) const {
    return asRect().contains(x, y);
}

bool Bounds::isInLimits(float x, float y) const {
    if (x<0) return false;
    if (y<0) return false;
    uint32_t ux = (uint32_t)x;
    uint32_t uy = (uint32_t)y;
    return ux < _dimension.getWidth() && uy < _dimension.getHeight();
}

void Bounds::shift(float dx, float dy) {
    _position.shift(Platec::Vector2D<float_t>(dx, dy));
    if(!_worldDimension.contains(_position))
    {
        
        float_t xval = _position.x(), yval = _position.y();
        
        if(std::floor(xval) < 0)
        {
            xval += static_cast<float_t>(_worldDimension.getWidth());
        }
        else if (std::floor(xval) > _worldDimension.getWidth())
        {
            xval -= static_cast<float_t>(_worldDimension.getWidth());
        }
        
        if(std::floor(yval) < 0)
        {
            yval += static_cast<float_t>(_worldDimension.getHeight());
        }
        else if (std::floor(yval) > _worldDimension.getHeight())
        {
            yval -= static_cast<float_t>(_worldDimension.getHeight());
        }
        _position = Platec::Point2D<float_t> (xval,yval);
    }
}

void Bounds::grow(uint32_t dx, uint32_t dy) {

    _dimension.grow(dx, dy);

    ASSERT(_dimension.getWidth() <= _worldDimension.getWidth(),
           "Bounds are larger than the world containing it");
    ASSERT(_dimension.getHeight() <= _worldDimension.getHeight(),
           "Bounds taller than the world containing it. delta=" + Platec::to_string(dy)
           + " resulting plate height=" + Platec::to_string(_dimension.getHeight())
           + " world height=" + Platec::to_string(_worldDimension.getHeight()));
}

Platec::Rectangle Bounds::asRect() const {
    const uint32_t ilft = leftAsUint();
    const uint32_t itop = topAsUint();
    const uint32_t irgt = ilft + _dimension.getWidth();
    const uint32_t ibtm = itop + _dimension.getHeight();

    return Platec::Rectangle(_worldDimension, ilft, irgt, itop, ibtm);
}

uint32_t Bounds::getMapIndex(uint32_t* px, uint32_t* py) const {
    return asRect().getMapIndex(px, py);
}

uint32_t Bounds::getValidMapIndex(uint32_t* px, uint32_t* py) const {
    uint32_t res = asRect().getMapIndex(px, py);
    ASSERT(res != BAD_INDEX, "BAD map index found");
    return res;
}
