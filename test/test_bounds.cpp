#include "bounds.hpp"
#include "gtest/gtest.h"

TEST(Bounds, Index)
{
  WorldDimension wd(800, 600);
  FloatPoint topLeft(10.2, 48.9);
  Dimension plateDim(500, 400);
  Bounds b(wd, topLeft, plateDim);
  EXPECT_EQ(0, b.index(0, 0));
  EXPECT_EQ(100100, b.index(100, 200));
  EXPECT_EQ(199999, b.index(499, 399));
}
