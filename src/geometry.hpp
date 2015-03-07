#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <stdexcept>
#include "utils.hpp"
#include "world_point.hpp"

using namespace std;

class WorldDimension;

class FloatPoint {
public:
    FloatPoint(float x, float y);
    float getX() const;
    float getY() const;
    void shift(float dx, float dy, const WorldDimension& _worldDimension);
private:
    float _x, _y;
};

class Dimension {
public:
    Dimension(uint32_t width, uint32_t height);
    Dimension(const Dimension& original);
    uint32_t getWidth() const;
    uint32_t getHeight() const;
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

class Vector
{
public:

private:
};

}

#endif
