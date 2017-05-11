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

uint32_t Bounds::left() const {
    return static_cast<uint32_t>(_position.x());
}

uint32_t Bounds::top() const {
    return static_cast<uint32_t>(_position.y());
}

uint32_t Bounds::bottom() const {
    return top()+ height();
}

uint32_t Bounds::right() const {
     return  left()+ width();
}


uint32_t Bounds::rightAsUintNonInclusive() const {
    return right() - 1;
}

uint32_t Bounds::bottomAsUintNonInclusive() const {
    return bottom() - 1;
}

bool Bounds::containsWorldPoint(const Platec::Point2D<uint32_t>& p) const {
    auto bot = bottom();
    auto rgt = right();
    if ( bottom() < top())
        bot += _worldDimension.getHeight();
    if ( right() < left())
        rgt += _worldDimension.getWidth();

    auto tmp = Platec::Point2D<uint32_t>(p.x() % _worldDimension.getWidth(),
                                         p.y() % _worldDimension.getHeight());

    bool x1 = (tmp.x() >= left()) && (tmp.x() < rgt);
    bool x2 = (tmp.x() + _worldDimension.getWidth() >= left())
           && (tmp.x() + _worldDimension.getWidth() < rgt);
    bool y1 = (tmp.y() >= top()) && (tmp.y() < bot);
    bool y2 = (tmp.y() +_worldDimension.getHeight() >= top())
           && (tmp.y() +_worldDimension.getHeight() < bot);

    // check if coordinates in bounds
    if ( ( x1 || x2) && ( y1 || y2)  ){
        return true;
    }
    return false;
}

bool Bounds::isInLimits(const Platec::Point2D<uint32_t>& p) const {

    return _dimension.contains(p);
}

void Bounds::shift(const Platec::Vector2D<float_t>& delta) {
    _position.shift(delta);
    if ( !_worldDimension.contains(_position)) {
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


std::pair<uint32_t, Platec::Point2D<uint32_t>>
        Bounds::getMapIndex(const Platec::Point2D<uint32_t>& p) const {
     // check if coordinates in bounds
    if (containsWorldPoint(p)) {
       auto tmp = Platec::Point2D<uint32_t>(p.x() % _worldDimension.getWidth(),
                                           p.y() % _worldDimension.getHeight());
       // calculate coordinates in Bounds
       const auto x = tmp.x() + ((tmp.x() < left())
                            ? _worldDimension.getWidth() : 0) - left();
       const auto y = tmp.y() + ((tmp.y() < top())
                            ? _worldDimension.getHeight() : 0) - top();

       return std::make_pair(_dimension.indexOf(x, y),
                    Platec::Point2D<uint32_t>(x, y));
    } else {
        // return bad index
       return std::make_pair(BAD_INDEX, p);
    }
}

std::pair<uint32_t, Platec::Point2D<uint32_t>>
        Bounds::getValidMapIndex(const Platec::Point2D<uint32_t>& p) const {
    auto res = getMapIndex(p);
    ASSERT(res.first != BAD_INDEX, "BAD map index found");

    return res;
}
