#include "bounds.hpp"

uint32_t Bounds::getMapIndex(uint32_t* px, uint32_t* py) const
{
    return asRect().getMapIndex(px, py);       
}

uint32_t Bounds::getValidMapIndex(uint32_t* px, uint32_t* py) const
{
    uint32_t res = asRect().getMapIndex(px, py);
    if (res == BAD_INDEX) {
        throw runtime_error("BAD INDEX found");
    }
    return res;
}
