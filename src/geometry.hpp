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

#define NOMINMAX

#include <cmath>
#include <stdexcept>
#include <algorithm>
#include "utils.hpp"
#include "world_point.hpp"


class WorldDimension;

namespace Platec {
class IntVector
{
private:    
    int x_value, y_value;
public:
    IntVector(int32_t x, int32_t y) : x_value(x), y_value(y) {}
    int32_t x() const {
        return x_value;
    }
    int32_t y() const {
        return y_value;
    }
    float_t length() const {
        return std::hypot(static_cast<float_t>(x_value),static_cast<float_t>(y_value));
        
    }
    friend IntVector operator-(const IntVector& a, const IntVector& b) {
        return IntVector(a.x() - b.x(), a.y() - b.y());
    }

};
}

/// A point with int coordinates.
class IntPoint {
    
private:
     int32_t x_value, y_value;
    
public:
    /// Create a point with the given coordinates
    IntPoint(int32_t x, int32_t y);

    /// X coordinate of the point
    int32_t getX() const;

    /// Y coordinate of the point
    int32_t getY() const;

    friend Platec::IntVector operator-(const IntPoint& a, const IntPoint& b) {
        return Platec::IntVector(a.getX() - b.getX(), a.getY() - b.getY());
    }
};

namespace Platec {
class FloatVector
{
private:
    float_t x_value, y_value;
    
public:
    FloatVector(float_t x, float_t y) : x_value(x), y_value(y) {}
    float_t x() const {
        return x_value;
    }
    float_t y() const {
        return y_value;
    }
    float_t length() const {
        return std::hypot(x_value,y_value);
    }
    float_t normalize() 
    {
        float_t len = length();
        if (len > 0.0f) 
        {
            x_value /= len;
            y_value /= len;
        }
        return len;
    }
    IntVector toIntVector() const {
        return IntVector(static_cast<int32_t>(x_value),static_cast<int32_t>(y_value));
    }
    float_t dotProduct(const FloatVector& other) const {
        return x() * other.x() + y() * other.y();
    }
    friend bool operator==(const FloatVector& a, const FloatVector& b) {
        return a.x() == b.x() && a.y() == b.y();
    }
    friend FloatVector operator-(const FloatVector& a, const FloatVector& b) {
        return FloatVector(a.x() - b.x(), a.y() - b.y());
   }
    friend FloatVector operator*(const FloatVector& v, float_t f) {
        return FloatVector(v.x() * f, v.y() * f);
    }

};
}

/// A point with float coordinates.
class FloatPoint 
{
private:

    float_t x_value, y_value;
public:

    /// Create a point with the given coordinates
    FloatPoint(float_t x, float_t y);

    /// X coordinate of the point
    float_t getX() const;

    /// Y coordinate of the point
    float_t getY() const;

    /// Move a point by the given delta, wrapping it around the borders of
    /// the world if needed.
    /// The given point is assured to be contained in the World.
    void shift(float_t dx, float_t dy, const WorldDimension& _worldDimension);

    /// Translate to an IntPoint (using the truncate operation).
    IntPoint toInt() const {
        return IntPoint(static_cast<int32_t>(x_value),static_cast<int32_t>(y_value));
    }
    friend Platec::FloatVector operator-(const FloatPoint& a, const FloatPoint& b) {
        return Platec::FloatVector(a.getX() - b.getX(), a.getY() - b.getY());
    }

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
