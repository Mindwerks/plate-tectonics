#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

#include <cstring> // for size_t
#include <stdexcept>

using namespace std;

class WorldDimension {
public:

    WorldDimension(size_t width, size_t height) :
        _width(width), _height(height)
    {
    };

    WorldDimension(const WorldDimension& original) :
        _width(original.getWidth()), _height(original.getHeight())
    {
    };

    WorldDimension operator=(const WorldDimension& original)
    {
        WorldDimension wd(original);
        return wd;
    };

    size_t getWidth() const
    {
        return _width;
    };

    size_t getHeight() const
    {
        return _height;
    };

    size_t getMax() const
    {
        return _width > _height ? _width : _height;
    }

    size_t getArea() const
    {
        return _width * _height;
    }

    size_t xMod(size_t x) const
    {
        return x % getWidth();
    }

    size_t yMod(size_t y) const
    {
        return y % getHeight();
    }    

    bool contains(const size_t x, const size_t y) const
    {
        return (x < _width && y < _height);
    }

    void normalize(size_t& x, size_t& y) const
    {
        x %= _width;
        y %= _height;
    }

    size_t indexOf(const size_t x, const size_t y) const
    {
        return y * getWidth() + x;
    }

    size_t lineIndex(const size_t y) const
    {
        if (y>=_height){
            throw invalid_argument("WorldDimension::line_index: y is not valid");
        }
        return indexOf(0, y);
    }

    size_t yFromIndex(const size_t index) const
    {
        return index / _width;
    }

    size_t xFromIndex(const size_t index) const
    {
        const size_t y = yFromIndex(index);
        return index - y * _width;
    }    

    size_t normalizedIndexOf(const size_t x, const size_t y) const
    {
        return indexOf(xMod(x), yMod(y));
    }

    size_t xCap(const size_t x) const
    {
        return x < _width ? x : (_width-1);
    }

    size_t yCap(const size_t y) const
    {
        return y < _height ? y : (_height-1);
    }

    size_t largerSize() const
    {
        return _width > _height ? _width : _height;
    }

private:
    const size_t _width;
    const size_t _height;
};

class Rectangle {
public:
    Rectangle(const WorldDimension& worldDimension,
            size_t left, size_t right,
            size_t top, size_t bottom)
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


    size_t getMapIndex(size_t* px, size_t* py) const throw();
    void enlarge_to_contain(size_t x, size_t y);

    size_t getLeft() const
    {
        return _left;
    }

    size_t getRight() const
    {
        return _right;
    }

    size_t getTop() const
    {
        return _top;
    }

    size_t getBottom() const
    {
        return _bottom;
    }

    void setLeft(size_t v)
    {
        _left = v;
    }

    void setRight(size_t v)
    {
        _right = v;
    }

    void setTop(size_t v)
    {
        _top = v;
    }

    void setBottom(size_t v)
    {
        _bottom = v;
    }

    void shift(size_t dx, size_t dy)
    {
        _left   += dx;
        _right  += dx;
        _top    += dy;
        _bottom += dy;
    }

private:
    const WorldDimension _worldDimension;
    size_t _left, _right;
    size_t _top, _bottom;
};

#endif
