#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

#include <cstring> // for size_t

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

    size_t getLeft()
    {
        return _left;
    }

    size_t getRight()
    {
        return _right;
    }

    size_t getTop()
    {
        return _top;
    }

    size_t getBottom()
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
