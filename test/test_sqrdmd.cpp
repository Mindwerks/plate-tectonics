#include "sqrdmd.hpp"
#include <cstdio>
#include "gtest/gtest.h"

TEST(SqrDmd, DoNotExplodeWithWidthLargerThanHeight)
{
    float map[100 * 20];
    sqrdmd(map, 100, 20, 0.5);
}

TEST(SqrDmd, DoNotExplodeWithHeightLargerThanWidth)
{
    float map[20 * 100];
    sqrdmd(map, 20, 100, 0.5);
}