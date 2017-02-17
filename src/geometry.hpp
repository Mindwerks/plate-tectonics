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
#include <type_traits>
#include "utils.hpp"
#include "world_point.hpp"


class WorldDimension;

namespace Platec {
   
template<class T, class Enable = void>
class Vector2D {};
    
template<class T>  
class Vector2D<T, typename std::enable_if<std::is_signed<T>::value>::type>
{
private:    
    T x_value, y_value;
public:
    Vector2D(T x, T y) : x_value(x), y_value(y) {}
    T x() const {
        return x_value;
    }
    T y() const {
        return y_value;
    }
    float_t length() const 
    {
        return std::hypot(static_cast<float_t>(x_value),static_cast<float_t>(y_value));
    }
    
    friend Vector2D<T> operator-(const Vector2D<T>& a, const Vector2D<T>& b) 
    {
        return Vector2D<T>(a.x() - b.x(), a.y() - b.y());
    }
    
    T dotProduct(const Vector2D<T>& other) const 
    {
        return x() * other.x() + y() * other.y();
    }
    
    friend bool operator==(const Vector2D<T>& a, const Vector2D<T>& b) 
    {
        return a.x() == b.x() && a.y() == b.y();
    }
    
    friend Vector2D<T> operator+(const Vector2D<T>& a, const Vector2D<T>& b)
    {
        return Vector2D<T>(a.x() + b.x(), a.y() + b.y());
    }
    
    friend Vector2D<T> operator*(const Vector2D<T>& v, T f)
    {
        return Vector2D<T>(v.x() * f, v.y() * f);
    }
};
    
template<class T, class Enable = void>
class Point2D {};
    
template<class T>  
class Point2D<T, typename std::enable_if<std::is_arithmetic<T>::value>::type>
{

private:
     T x_value, y_value;
    
public:
    /// Create a point with the given coordinates
    Point2D(T x, T y): x_value(x), y_value(y) {};

    /// X coordinate of the point
    T x() const
    {
        return x_value;
    };

    /// Y coordinate of the point
    T y() const
    {
        return y_value;
    }

    friend Vector2D<T> operator-(const Point2D<T>& a, const Point2D<T>& b) 
    {
        return Vector2D<T>(a.x() - b.x(), a.y() - b.y());
    }
    
    template<class R>
    void shift(const Vector2D<T>& delta, const Point2D<R>& _worldDimension)
    {
        //TODO. Shift should be replace with the + operator.
        // Boundchecking is not something a Point should do,
        // but we have to solve the int <-> float issue here first
        const uint32_t world_width = _worldDimension.x();
        x_value += delta.x();
        x_value += x_value > 0 ? 0.0f : world_width;
        x_value -= x_value < world_width ? 0.0f  : world_width;

        const uint32_t world_height = _worldDimension.y();
        y_value += delta.y();
        y_value += y_value > 0 ? 0.0f  : world_height;
        y_value -= y_value < world_height ? 0.0f  : world_height;
    }  
};

}

/// Dimension of a Rectangle.
class Dimension {
    
protected:
    uint32_t _width;
    uint32_t _height;
public:

    /// Initialize the dimension with the given values
    Dimension(uint32_t width, uint32_t height);

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
    bool contains(const Platec::Point2D<float_t>& p) const;
    void grow(uint32_t amountX, uint32_t amountY);

};

class WorldDimension : public Dimension {
public:
    WorldDimension(uint32_t width, uint32_t height);
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
