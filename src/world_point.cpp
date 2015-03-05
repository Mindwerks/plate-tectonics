#include "world_point.hpp"
#include "rectangle.hpp"

WorldPoint::WorldPoint(uint32_t x, uint32_t y, const WorldDimension& worldDimension)
    : _x(x), _y(y)
{
    if (_x >= worldDimension.getWidth()) {
        throw runtime_error("WorldPoint::WorldPoint");
    }
    if (_y >= worldDimension.getHeight()) {
        throw runtime_error("WorldPoint::WorldPoint");
    }   
}

WorldPoint::WorldPoint(const WorldPoint& other)
    : _x(other.x()),
      _y(other.y())
{ }

uint32_t WorldPoint::x() const
{
    return _x;
}

uint32_t WorldPoint::y() const
{
    return _y;
}

uint32_t WorldPoint::toIndex(const WorldDimension& dim) const
{
    if (_x >= dim.getWidth()) {
        throw runtime_error("WorldPoint::WorldPoint");
    }
    if (_y >= dim.getHeight()) {
        throw runtime_error("WorldPoint::WorldPoint");
    }   
    return _y * dim.getWidth() + _x;
}
