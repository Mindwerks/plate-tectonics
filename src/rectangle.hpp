#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

#include <cstring> // for size_t
#include <stdexcept>
#include "utils.hpp"
#include "geometry.hpp"

using namespace std;

#define BAD_INDEX 0xFFFFFFFF

namespace Platec {

class Rectangle {
public:
    Rectangle(const WorldDimension& worldDimension,
            uint32_t left, uint32_t right,
            uint32_t top, uint32_t bottom)
            : _worldDimension(worldDimension),
              _left(left), _right(right),
              _top(top), _bottom(bottom)
    {
    };

    Rectangle(const Rectangle& original) :
        _worldDimension(original._worldDimension),
        _left(original._left), _right(original._right),
        _top(original._top), _bottom(original._bottom)
    {
    };

    Rectangle operator=(const Rectangle& original)
    {
        Rectangle r(original);
        return r;
    };

    uint32_t getMapIndex(uint32_t* px, uint32_t* py) const throw();
    void enlarge_to_contain(uint32_t x, uint32_t y);

    uint32_t getLeft() const
    {
        return _left;
    }

    uint32_t getRight() const
    {
        return _right;
    }

    uint32_t getTop() const
    {
        return _top;
    }

    uint32_t getBottom() const
    {
        return _bottom;
    }

    void setLeft(uint32_t v)
    {
        _left = v;
    }

    void setRight(uint32_t v)
    {
        _right = v;
    }

    void setTop(uint32_t v)
    {
        _top = v;
    }

    void setBottom(uint32_t v)
    {
        _bottom = v;
    }

    void shift(uint32_t dx, uint32_t dy)
    {
        _left   += dx;
        _right  += dx;
        _top    += dy;
        _bottom += dy;
    }

    bool contains(uint32_t x, uint32_t y) const
    {
        uint32_t cleanX = _worldDimension.xMod(x);
        uint32_t cleanY = _worldDimension.yMod(y);
        if (cleanX < getLeft()) cleanX += _worldDimension.getWidth();
        if (cleanY < getTop()) cleanY += _worldDimension.getHeight();
        return cleanX >= getLeft() && cleanX < getRight() 
            && cleanY >= getTop()  && cleanY < getBottom();
    }

private:
    const WorldDimension _worldDimension;
    uint32_t _left, _right;
    uint32_t _top, _bottom;
};

};


#endif
