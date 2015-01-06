#include "sqrdmd.hpp"
#include <cstdio>
#include "gtest/gtest.h"

TEST(SqrDmd, DoNotExplodeWithWidthLargerThanHeight)
{
    long seed = 3;
    float map[128 * 32];
    sqrdmd(seed, map, 128, 32, 0.5);
}

TEST(SqrDmd, DoNotExplodeWithHeightLargerThanWidth)
{
    long seed = 3;
    float map[32 * 128];
    sqrdmd(seed, map, 32, 128, 0.5);
}