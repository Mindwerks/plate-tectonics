#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

class Rectangle {
public:
    Rectangle(size_t world_width, size_t world_height,
            size_t left, size_t right,
            size_t top, size_t bottom)
            : _world_width(world_width), _world_height(world_height),
              _left(left), _right(right),
              _top(top), _bottom(bottom)
    {
    }

private:
    size_t _world_width, _world_height;
    size_t _left, _right;
    size_t _top, _bottom;
}

#endif