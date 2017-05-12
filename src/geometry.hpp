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
class Vector2D<T, typename std::enable_if<std::is_arithmetic<T>::value>::type>
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
    
    void shift(const Vector2D<T>& delta)
    {
        x_value += delta.x();
        y_value += delta.y();
    }  
};

}

/// Dimension of a Rectangle.
class Dimension {
    
protected:
    Platec::Vector2D<uint32_t> dim;
public:

    /// Initialize the dimension with the given values
    Dimension(const uint32_t width,const uint32_t height);

    uint32_t getWidth() const {
        return dim.x();
    }
    uint32_t getHeight() const {
        return dim.y();
    }
    uint32_t getArea() const {
        return getWidth() * getHeight();
    }
    uint32_t indexOf(const uint32_t x, const uint32_t y) const;
    uint32_t indexOf(const Platec::Point2D<uint32_t>& point) const;
    
    uint32_t yFromIndex(const uint32_t index) const;
    uint32_t xFromIndex(const uint32_t index) const;    
    
    const Platec::Vector2D<uint32_t> coordOF(const uint32_t index) const;

    template <class T>
    bool contains(const Platec::Point2D<T>& p) const {
        //using std::floor here to avoid floating point inaccuarcy
       return (std::floor(p.x()) >= 0 && std::floor(p.x()) < getWidth()  
               && std::floor(p.y()) >= 0.0f && std::floor(p.y()) < getHeight());
    }
    void grow(Platec::Vector2D<uint32_t> growSize);

};

class WorldDimension : public Dimension {
public:
    WorldDimension(const uint32_t width,const uint32_t height);
    uint32_t getMax() const;
    uint32_t xMod(const uint32_t x) const;
    Platec::Point2D<uint32_t> xMod(const Platec::Point2D<uint32_t>& point) const;
    uint32_t yMod(const uint32_t y) const;
    Platec::Point2D<uint32_t> yMod(const Platec::Point2D<uint32_t>& point) const;
    Platec::Point2D<uint32_t> pointMod(const Platec::Point2D<uint32_t>& point) const;
    Platec::Point2D<uint32_t>  normalize(const Platec::Point2D<uint32_t>& point) const;
    uint32_t lineIndex(const uint32_t y) const;
    uint32_t normalizedIndexOf(const uint32_t x, const uint32_t y) const;
    uint32_t normalizedIndexOf(const Platec::Point2D<uint32_t>& point) const; 
    uint32_t xCap(const uint32_t x) const;
    Platec::Point2D<uint32_t> xCap(const Platec::Point2D<uint32_t>& point) const;
    uint32_t yCap(const uint32_t y) const;
    Platec::Point2D<uint32_t> yCap(const Platec::Point2D<uint32_t>& point) const;
    
    template <class T>
    Platec::Point2D<T> wrap(const Platec::Point2D<T>& point) const
    {
        T xval = point.x(), yval = point.y();
        if(std::floor(xval) < 0)
        {
            xval += static_cast<T>(getWidth());
        }
        else if (std::floor(xval) > getWidth())
        {
            xval -= static_cast<T>(getWidth());
        }
        
        if(std::floor(yval) < 0)
        {
            yval += static_cast<T>(getHeight());
        }
        else if (std::floor(yval) > getHeight())
        {
            yval -= static_cast<T>(getHeight());
        }
        return Platec::Point2D<T> (xval,yval);
    }

};

#endif
