#ifndef WORLD_POINT_HPP
#define WORLD_POINT_HPP

#include "utils.hpp"

class WorldDimension;

/// Immutable point expressed in World coordinates
class WorldPoint
{
public:
    WorldPoint(uint32_t x, uint32_t y);
    WorldPoint(const WorldPoint& other);
    uint32_t x() const;
    uint32_t y() const;
    uint32_t toIndex(const WorldDimension&) const;
private:
    const uint32_t _x;
    const uint32_t _y;
};

#endif
