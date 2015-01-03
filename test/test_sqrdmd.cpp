#include "sqrdmd.hpp"
#include <cstdio>
#include "gtest/gtest.h"

TEST(SqrDmd, DoNotExplodeWithWidthLargerThanHeight)
{
    long seed = 3;
    float map[100 * 20];
    sqrdmd(seed, map, 100, 20, 0.5);
}

TEST(SqrDmd, DoNotExplodeWithHeightLargerThanWidth)
{
    long seed = 3;
    float map[20 * 100];
    sqrdmd(seed, map, 20, 100, 0.5);
}