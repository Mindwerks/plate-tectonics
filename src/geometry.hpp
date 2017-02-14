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

#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <math.h>
#include <stdexcept>
#include "utils.hpp"
#include "world_point.hpp"

using namespace std;

class WorldDimension;

namespace Platec {
class IntVector
{
public:
    IntVector(int x, int y) : _x(x), _y(y) {}
    int x() const {
        return _x;
    }
    int y() const {
        return _y;
    }
    float length() const {
        return sqrt((float)(_x * _x + _y * _y));
    }
    friend IntVector operator-(const IntVector& a, const IntVector& b) {
        return IntVector(a.x() - b.x(), a.y() - b.y());
    }
private:
    int _x, _y;
};
}

/// A point with int coordinates.
class IntPoint {
public:

    /// Create a point with the given coordinates
    IntPoint(int x, int y);

    /// X coordinate of the point
    int getX() const;

    /// Y coordinate of the point
    int getY() const;

    friend Platec::IntVector operator-(const IntPoint& a, const IntPoint& b) {
        return Platec::IntVector(a.getX() - b.getX(), a.getY() - b.getY());
    }
private:

    int _x, _y;
};

namespace Platec {
class FloatVector
{
public:
    FloatVector(float x, float y) : _x(x), _y(y) {}
    float x() const {
        return _x;
    }
    float y() const {
        return _y;
    }
    float length() const {
        return sqrt(_x * _x + _y * _y);
    }
    float normalize() {
        float len = length();
        if (len > 0) {
            float inv_len = 1 / len;
            _x *= inv_len;
            _y *= inv_len;
        }
        return len;
    }
    IntVector toIntVector() const {
        return IntVector((int)_x, (int)_y);
    }
    float dotProduct(const FloatVector& other) const {
        return x() * other.x() + y() * other.y();
    }
    friend bool operator==(const FloatVector& a, const FloatVector& b) {
        return a.x() == b.x() && a.y() == b.y();
    }
    friend FloatVector operator-(const FloatVector& a, const FloatVector& b) {
        return FloatVector(a.x() - b.x(), a.y() - b.y());
    }
    friend FloatVector operator*(const FloatVector& v, float f) {
        return FloatVector(v.x() * f, v.y() * f);
    }
private:
    float _x, _y;
};
}

/// A point with float coordinates.
class FloatPoint {
public:

    /// Create a point with the given coordinates
    FloatPoint(float x, float y);

    /// X coordinate of the point
    float getX() const;

    /// Y coordinate of the point
    float getY() const;

    /// Move a point by the given delta, wrapping it around the borders of
    /// the world if needed.
    /// The given point is assured to be contained in the World.
    void shift(float dx, float dy, const WorldDimension& _worldDimension);

    /// Translate to an IntPoint (using the truncate operation).
    IntPoint toInt() const {
        return IntPoint((int)_x, (int)_y);
    }
    friend Platec::FloatVector operator-(const FloatPoint& a, const FloatPoint& b) {
        return Platec::FloatVector(a.getX() - b.getX(), a.getY() - b.getY());
    }
private:

    float _x, _y;
};

/// Dimension of a Rectangle.
class Dimension {
public:

    /// Initialize the dimension with the given values
    Dimension(uint32_t width, uint32_t height);
    Dimension(const Dimension& original);

    uint32_t getWidth() const {
        return _width;
    }
    uint32_t getHeight() const {
        return _height;
    }
    uint32_t getArea() const {
        return _width * _height;
    }
    bool contains(const uint32_t x, const uint32_t y) const;
    bool contains(const float x, const float y) const;
    bool contains(const FloatPoint& p) const;
    void grow(uint32_t amountX, uint32_t amountY);
protected:
    uint32_t _width;
    uint32_t _height;
};

class WorldDimension : public Dimension {
public:
    WorldDimension(uint32_t width, uint32_t height);
    WorldDimension(const WorldDimension& original);
    uint32_t getMax() const;
    uint32_t xMod(uint32_t x) const;
    uint32_t yMod(uint32_t y) const;
    void normalize(uint32_t& x, uint32_t& y) const;
    uint32_t indexOf(const uint32_t x, const uint32_t y) const;
    uint32_t lineIndex(const uint32_t y) const;
    uint32_t yFromIndex(const uint32_t index) const;
    uint32_t xFromIndex(const uint32_t index) const;
    uint32_t normalizedIndexOf(const uint32_t x, const uint32_t y) const;
    uint32_t xCap(const uint32_t x) const;
    uint32_t yCap(const uint32_t y) const;
};

#endif
