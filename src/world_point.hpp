#ifndef WORLD_POINT_HPP
#define WORLD_POINT_HPP

class WorldDimension;

class Position
{
public:
    Position(unsigned int x, unsigned int y);
    Position(const Position& other);
    unsigned int x() const;
    unsigned int y() const;
    unsigned int toIndex(const WorldDimension&) const;
private:
    unsigned int _x;
    unsigned int _y;
};

#endif
