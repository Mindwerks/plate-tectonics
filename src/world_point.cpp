#include "world_point.hpp"
#include "rectangle.hpp"

WorldPoint::WorldPoint(uint32_t x, uint32_t y)
    : _x(x),
      _y(y)
{
}

WorldPoint::WorldPoint(const WorldPoint& other)
    : _x(other.x()),
      _y(other.y())
{

}

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
    return _y * dim.getWidth() + _x;
}
