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

uint32_t Bounds::index(const Platec::Point2D<uint32_t>& p) const {
    ASSERT(_dimension.contains(p),
           "Invalid coordinates");
    return _dimension.indexOf(p);
    
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
    return static_cast<uint32_t>(_position.x());
}

uint32_t Bounds::topAsUint() const {
    return static_cast<uint32_t>(_position.y());
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

bool Bounds::isInLimits(const Platec::Point2D<uint32_t>& p) const {

    return _dimension.contains(p);
}

void Bounds::shift(const Platec::Vector2D<float_t>& delta) {
    _position.shift(delta);
    if(!_worldDimension.contains(_position))
    {
        _position = _worldDimension.wrap(_position);
    }
}

void Bounds::grow(const Platec::Vector2D<uint32_t>& delta) {

    _dimension.grow(delta);
  //  _worldDimension.contains(_dimension.) TODO
    ASSERT(_dimension.getWidth() <= _worldDimension.getWidth(),
           "Bounds are larger than the world containing it");
    ASSERT(_dimension.getHeight() <= _worldDimension.getHeight(),
           "Bounds taller than the world containing it. delta=" + Platec::to_string(delta.y())
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
