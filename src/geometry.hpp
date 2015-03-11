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

#include <stdexcept>
#include "utils.hpp"
#include "world_point.hpp"

using namespace std;

class WorldDimension;

/// A point with int coordinates.
class IntPoint {
public:
    
    /// Create a point with the given coordinates
    IntPoint(int x, int y);

    /// X coordinate of the point
    int getX() const;

    /// Y coordinate of the point
    int getY() const;

private:

    int _x, _y;
};

/// Create a new point by subtracting the coordinates of the second
/// point from the coordinate of the first.
IntPoint operator-(const IntPoint& a, const IntPoint& b);


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
    IntPoint toInt() const;

private:

    float _x, _y;
};

/// Dimension of a Rectangle.
class Dimension {
public:

    /// Initialize the dimension with the given values
    Dimension(uint32_t width, uint32_t height);
    Dimension(const Dimension& original);
    
    /// Width of the rectangle
    uint32_t getWidth() const;

    /// Height of the rectangle
    uint32_t getHeight() const;

    /// Area of the rectangle
    uint32_t getArea() const;
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
    uint32_t xMod(int x) const;
    uint32_t yMod(int y) const;
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
    uint32_t largerSize() const;
};	

namespace Platec {

class IntVector;
class FloatVector;

class IntVector
{
public:
    IntVector(int x, int y);
    int x() const { return _x; }
    int y() const { return _y; }
    static IntVector fromDistance(const IntPoint& a, const IntPoint& b);
    float length() const;
    FloatVector toUnitVector() const;
private:
    int _x, _y;
};    

IntVector operator-(const IntVector& a, const IntVector& b);

class FloatVector
{
public:
    FloatVector(float x, float y);
    float x() const { return _x; }
    float y() const { return _y; }
    float length() const;
    IntVector toIntVector() const;
    float dotProduct(const FloatVector& other) const;
private:
    float _x, _y;
};

bool operator==(const FloatVector& a, const FloatVector& b);
FloatVector operator-(const FloatVector& a, const FloatVector& b);
FloatVector operator*(const FloatVector& v, float f);

}

#endif
