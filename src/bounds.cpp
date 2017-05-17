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
#include <utility>

Bounds::Bounds(const Dimension& worldDimension,
                const Platec::vec2f& position,
               const Dimension& dimension)
    : worldDimension(worldDimension),
      position(position),
      dimension(dimension) {
    ASSERT(dimension.getWidth() <= worldDimension.getWidth() &&
           dimension.getHeight() <= worldDimension.getHeight(),
           "Bounds are larger than the world containing it");
}

uint32_t Bounds::index(const Platec::vec2ui& p) const {
    ASSERT(dimension.contains(p),
           "Invalid coordinates");
    return dimension.indexOf(p);
}

uint32_t Bounds::area() const {
    return dimension.getArea();
}

uint32_t Bounds::width() const {
    return dimension.getWidth();
}

uint32_t Bounds::height() const {
    return dimension.getHeight();
}

uint32_t Bounds::left() const {
    return static_cast<uint32_t>(position.x());
}

uint32_t Bounds::top() const {
    return static_cast<uint32_t>(position.y());
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

bool Bounds::containsWorldPoint(const Platec::vec2ui& p) const {
    auto bot = bottom();
    auto rgt = right();
    if ( bottom() < top())
        bot += worldDimension.getHeight();
    if ( right() < left())
        rgt += worldDimension.getWidth();

    auto tmp = Platec::vec2ui(p.x() % worldDimension.getWidth(),
                                         p.y() % worldDimension.getHeight());

    bool x1 = (tmp.x() >= left()) && (tmp.x() < rgt);
    bool x2 = (tmp.x() + worldDimension.getWidth() >= left())
           && (tmp.x() + worldDimension.getWidth() < rgt);
    bool y1 = (tmp.y() >= top()) && (tmp.y() < bot);
    bool y2 = (tmp.y() +worldDimension.getHeight() >= top())
           && (tmp.y() +worldDimension.getHeight() < bot);

    // check if coordinates in bounds
    if ((x1 || x2) && (y1 || y2)) {
        return true;
    }
    return false;
}

bool Bounds::isInLimits(const Platec::vec2ui& p) const {
    return dimension.contains(p);
}

void Bounds::shift(const Platec::vec2f& delta) {
    position.shift(delta);
    if (!worldDimension.contains(position)) {
        position = worldDimension.wrap(position);
    }
}

void Bounds::grow(const Platec::vec2ui& delta) {
    dimension.grow(delta);
  //  _worldDimension.contains(_dimension.) TODO
    ASSERT(dimension.getWidth() <= worldDimension.getWidth(),
           "Bounds are larger than the world containing it");
    ASSERT(dimension.getHeight() <= worldDimension.getHeight(),
           "Bounds taller than the world containing it. delta="
            + Platec::to_string(delta.y())
           + " resulting plate height="
            + Platec::to_string(dimension.getHeight())
           + " world height=" + Platec::to_string(worldDimension.getHeight()));
}


std::pair<uint32_t, Platec::vec2ui>
        Bounds::getMapIndex(const Platec::vec2ui& p) const {
     // check if coordinates in bounds
    if (containsWorldPoint(p)) {
       auto tmp = Platec::vec2ui(p.x() % worldDimension.getWidth(),
                                           p.y() % worldDimension.getHeight());
       // calculate coordinates in Bounds
       const auto x = tmp.x() + ((tmp.x() < left())
                            ? worldDimension.getWidth() : 0) - left();
       const auto y = tmp.y() + ((tmp.y() < top())
                            ? worldDimension.getHeight() : 0) - top();

       tmp = Platec::vec2ui(x, y);
       return std::make_pair(dimension.indexOf(tmp),tmp);
    } else {
        // return bad index
       return std::make_pair(BAD_INDEX, p);
    }
}

std::pair<uint32_t, Platec::vec2ui>
        Bounds::getValidMapIndex(const Platec::vec2ui& p) const {
    auto res = getMapIndex(p);
    ASSERT(res.first != BAD_INDEX, "BAD map index found");

    return res;
}
