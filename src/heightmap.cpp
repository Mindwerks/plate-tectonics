#include "heightmap.hpp"

#include <stdexcept> // std::invalid_argument

using namespace std;


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
