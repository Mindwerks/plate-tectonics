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

Bounds::Bounds(const WorldDimension& worldDimension, const FloatPoint& position,
       const Dimension& dimension)
    : _worldDimension(worldDimension),
      _position(position),
      _dimension(dimension)
{
    if (_dimension.getWidth() >= _worldDimension.getWidth()) {
        throw runtime_error("(Bounds::Bounds) Plate is larger than the world containing it");
    }
    if (_dimension.getHeight() > _worldDimension.getHeight()) {
        throw runtime_error("(Bounds::Bounds) Plate is taller than the world containing it");
    }   
}

uint32_t Bounds::index(uint32_t x, uint32_t y) const 
{
    if (x >= _dimension.getWidth()) {
        throw runtime_error("Bounds::Index: unvalid x coordinate");
    }
    if (y >= _dimension.getHeight()) {
        throw runtime_error("Bounds::Index: unvalid y coordinate");
    }
    return y * _dimension.getWidth() + x;
}   

uint32_t Bounds::area() const
{
    return _dimension.getArea();
}

uint32_t Bounds::width() const 
{
    return _dimension.getWidth();
}

uint32_t Bounds::height() const 
{
    return _dimension.getHeight();
}

uint32_t Bounds::leftAsUint() const 
{
    p_assert(_position.getX() >= 0, "_position.getX() should be not negative");
    return _position.getX();
}

uint32_t Bounds::topAsUint() const 
{
    p_assert(_position.getY() >= 0, "_position.getY() should be not negative");
    return _position.getY();
}   

uint32_t Bounds::rightAsUintNonInclusive() const 
{
    return leftAsUint() + width() - 1;
}

uint32_t Bounds::bottomAsUintNonInclusive() const 
{
    return topAsUint() + height() - 1;
}

bool Bounds::containsWorldPoint(uint32_t x, uint32_t y) const 
{
    return asRect().contains(x, y);
}

bool Bounds::isInLimits(float x, float y) const 
{
    if (x<0) return false;
    if (y<0) return false;
    uint32_t ux = x;
    uint32_t uy = y;
    return ux < _dimension.getWidth() && uy < _dimension.getHeight();
}

void Bounds::shift(float dx, float dy) {
    _position.shift(dx, dy, _worldDimension);
    p_assert(_worldDimension.contains(_position), "");
}

void Bounds::grow(int dx, int dy)
{
    if (dx<0) throw runtime_error("negative value");
    if (dy<0) throw runtime_error("negative value");
    _dimension.grow(dx, dy);

    if (_dimension.getWidth() > _worldDimension.getWidth()) {
        throw runtime_error("(Bounds::grow) Plate is larger than the world containing it");
    }    
    if (_dimension.getHeight() > _worldDimension.getHeight()) {
        string s("(Bounds::grow) Plate is taller than the world containing it:");
        s += " delta=" + Platec::to_string(dy);
        s += " resulting plate height=" + Platec::to_string(_dimension.getHeight());
        s += " world height=" + Platec::to_string(_worldDimension.getHeight());
        throw runtime_error(s);
    }     
}

Platec::Rectangle Bounds::asRect() const 
{
    p_assert(_position.getX() > 0.0f && _position.getY() >= 0.0f, "Left and top must be positive");  
    const uint32_t ilft = leftAsUint();
    const uint32_t itop = topAsUint();
    const uint32_t irgt = ilft + _dimension.getWidth();
    const uint32_t ibtm = itop + _dimension.getHeight();

    return Platec::Rectangle(_worldDimension, ilft, irgt, itop, ibtm);     
}

uint32_t Bounds::getMapIndex(uint32_t* px, uint32_t* py) const
{
    return asRect().getMapIndex(px, py);       
}

uint32_t Bounds::getValidMapIndex(uint32_t* px, uint32_t* py) const
{
    uint32_t res = asRect().getMapIndex(px, py);
    if (res == BAD_INDEX) {
        throw runtime_error("BAD INDEX found");
    }
    return res;
}
