#include "world_point.hpp"
#include "rectangle.hpp"

Position::Position(unsigned int x, unsigned int y)
    : _x(x),
      _y(y)
{
};

Position::Position(const Position& other)
    : _x(other.x()),
      _y(other.y())
{

};

unsigned int Position::x() const
{
    return _x;
};

unsigned int Position::y() const
{
    return _y;
};

unsigned int Position::toIndex(const WorldDimension& dim) const
{
    return _y * dim.getWidth() + _x;
};
