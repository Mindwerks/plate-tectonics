#include "sqrdmd.hpp"
#include <cstdio>
#include "gtest/gtest.h"

TEST(SqrDmd, DoNotExplodeWithWidthLargerThanHeight)
{
    long seed = 3;
    float map[129 * 33];
    sqrdmd(seed, map, 129, 33, 0.5);
}

TEST(SqrDmd, DoNotExplodeWithHeightLargerThanWidth)
{
    long seed = 3;
    float map[33 * 129];
    sqrdmd(seed, map, 33, 129, 0.5);
}