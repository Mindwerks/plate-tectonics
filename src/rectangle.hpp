#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

#include <cstring> // for size_t

class Rectangle {
public:
    Rectangle(size_t world_width, size_t world_height,
            size_t left, size_t right,
            size_t top, size_t bottom)
            : _world_width(world_width), _world_height(world_height),
              _left(left), _right(right),
              _top(top), _bottom(bottom)
    {
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
    size_t _world_width, _world_height;
    size_t _left, _right;
    size_t _top, _bottom;
};

#endif
