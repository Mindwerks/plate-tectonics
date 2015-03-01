#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

#include <cstring> // for size_t
#include <stdexcept>
#include "utils.hpp"

using namespace std;

class Dimension {
public:
    Dimension(uint32_t width, uint32_t height) :
        _width(width), _height(height)
    {
    };

    Dimension(const Dimension& original) :
        _width(original.getWidth()), _height(original.getHeight())
    {
    };

    uint32_t getWidth() const
    {
        return _width;
    };

    uint32_t getHeight() const
    {
        return _height;
    };

    uint32_t getArea() const
    {
        return _width * _height;
    }

    bool contains(const uint32_t x, const uint32_t y) const
    {
        return (x >= 0 && x < _width && y >= 0 && y < _height);
    }

    void growWidth(uint32_t amount)
    {
        _width += amount;
    }

    void growHeight(uint32_t amount)
    {
        _height += amount;
    }

protected:
    uint32_t _width;
    uint32_t _height;
};

class WorldDimension : public Dimension {
public:

    WorldDimension(uint32_t width, uint32_t height) : Dimension(width, height)
    {
    };

    WorldDimension(const WorldDimension& original) : Dimension(original)
    {
    };

    uint32_t getMax() const
    {
        return _width > _height ? _width : _height;
    }

    uint32_t xMod(uint32_t x) const
    {
        return (x + _width) % getWidth();
    }

    uint32_t yMod(uint32_t y) const
    {
        return (y + _height) % getHeight();
    }    

    void normalize(uint32_t& x, uint32_t& y) const
    {
        x %= _width;
        y %= _height;
    }

    uint32_t indexOf(const uint32_t x, const uint32_t y) const
    {
        return y * getWidth() + x;
    }

    uint32_t lineIndex(const uint32_t y) const
    {
        if (y<0 || y>=_height){
            throw invalid_argument("WorldDimension::line_index: y is not valid");
        }
        return indexOf(0, y);
    }

    uint32_t yFromIndex(const uint32_t index) const
    {
        return index / _width;
    }

    uint32_t xFromIndex(const uint32_t index) const
    {
        const uint32_t y = yFromIndex(index);
        return index - y * _width;
    }    

    uint32_t normalizedIndexOf(const uint32_t x, const uint32_t y) const
    {
        return indexOf(xMod(x), yMod(y));
    }

    uint32_t xCap(const uint32_t x) const
    {
        return x < _width ? x : (_width-1);
    }

    uint32_t yCap(const uint32_t y) const
    {
        return y < _height ? y : (_height-1);
    }

    uint32_t largerSize() const
    {
        return _width > _height ? _width : _height;
    }

private:

};

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

private:
    const WorldDimension _worldDimension;
    uint32_t _left, _right;
    uint32_t _top, _bottom;
};

};

#endif
