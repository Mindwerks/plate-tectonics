#include "geometry.hpp"

//
// FloatPoint
//

FloatPoint::FloatPoint(float x, float y)
    : _x(x), _y(y)
{ }

float FloatPoint::getX() const
{
    return _x;
}

float FloatPoint::getY() const
{
    return _y;
}

void FloatPoint::shift(float dx, float dy, const WorldDimension& _worldDimension)
{
    _x += dx;
    _x += _x > 0 ? 0 : _worldDimension.getWidth();
    _x -= _x < _worldDimension.getWidth() ? 0 : _worldDimension.getWidth();

    _y += dy;
    _y += _y > 0 ? 0 : _worldDimension.getHeight();
    _y -= _y < _worldDimension.getHeight() ? 0 : _worldDimension.getHeight();

    p_assert(_worldDimension.contains(*this), "(FloatPoint::shift)");
}

//
// Dimension
//

Dimension::Dimension(uint32_t width, uint32_t height) :
    _width(width), _height(height)
{
}

Dimension::Dimension(const Dimension& original) :
    _width(original.getWidth()), _height(original.getHeight())
{
}

uint32_t Dimension::getWidth() const
{
    return _width;
}

uint32_t Dimension::getHeight() const
{
    return _height;
}

uint32_t Dimension::getArea() const
{
    return _width * _height;
}

bool Dimension::contains(const uint32_t x, const uint32_t y) const
{
    return (x >= 0 && x < _width && y >= 0 && y < _height);
}

bool Dimension::contains(const float x, const float y) const
{
    return (x >= 0 && x < _width && y >= 0 && y < _height);
}

bool Dimension::contains(const FloatPoint& p) const
{
    return (p.getX() >= 0 && p.getX() < _width && p.getY() >= 0 && p.getY() < _height);
}

void Dimension::grow(uint32_t amountX, uint32_t amountY)
{
    _width += amountX;
    _height += amountY;
}

//
// WorldDimension
//

WorldDimension::WorldDimension(uint32_t width, uint32_t height) : Dimension(width, height)
{
};

WorldDimension::WorldDimension(const WorldDimension& original) : Dimension(original)
{
};

uint32_t WorldDimension::getMax() const
{
    return _width > _height ? _width : _height;
}

uint32_t WorldDimension::xMod(int x) const
{
    return (x + _width) % getWidth();
}

uint32_t WorldDimension::yMod(int y) const
{
    return (y + _height) % getHeight();
}

uint32_t WorldDimension::xMod(uint32_t x) const
{
    return (x + _width) % getWidth();
}

uint32_t WorldDimension::yMod(uint32_t y) const
{
    return (y + _height) % getHeight();
}    

void WorldDimension::normalize(uint32_t& x, uint32_t& y) const
{
    x %= _width;
    y %= _height;
}

uint32_t WorldDimension::indexOf(const uint32_t x, const uint32_t y) const
{
    return y * getWidth() + x;
}

uint32_t WorldDimension::lineIndex(const uint32_t y) const
{
    if (y<0 || y>=_height){
        throw invalid_argument("WorldDimension::line_index: y is not valid");
    }
    return indexOf(0, y);
}

uint32_t WorldDimension::yFromIndex(const uint32_t index) const
{
    return index / _width;
}

uint32_t WorldDimension::xFromIndex(const uint32_t index) const
{
    const uint32_t y = yFromIndex(index);
    return index - y * _width;
}    

uint32_t WorldDimension::normalizedIndexOf(const uint32_t x, const uint32_t y) const
{
    return indexOf(xMod(x), yMod(y));
}

uint32_t WorldDimension::xCap(const uint32_t x) const
{
    return x < _width ? x : (_width-1);
}

uint32_t WorldDimension::yCap(const uint32_t y) const
{
    return y < _height ? y : (_height-1);
}

uint32_t WorldDimension::largerSize() const
{
    return _width > _height ? _width : _height;
}


namespace Platec {

}
