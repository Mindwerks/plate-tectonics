#include "bounds.hpp"
#include "gtest/gtest.h"

WorldDimension wd(800, 600);
FloatPoint topLeft(10.2, 48.9);
Dimension plateDim(500, 400);
Bounds b(wd, topLeft, plateDim);

TEST(Bounds, Index)
{
  EXPECT_EQ(0, b.index(0, 0));
  EXPECT_EQ(100100, b.index(100, 200));
  EXPECT_EQ(199999, b.index(499, 399));
}

TEST(Bounds, Area)
{
  EXPECT_EQ(200000, b.area());
}

TEST(Bounds, Width)
{
  EXPECT_EQ(500, b.width());
}

TEST(Bounds, Height)
{
  EXPECT_EQ(400, b.height());
}

TEST(Bounds, LeftAsUint)
{
  EXPECT_EQ(10, b.leftAsUint());
}

TEST(Bounds, TopAsUint)
{
  EXPECT_EQ(48, b.topAsUint());
}

TEST(Bounds, RightAsUintNonInclusive)
{
  EXPECT_EQ(509, b.rightAsUintNonInclusive());
}

TEST(Bounds, BottomAsUintNonInclusive)
{
  EXPECT_EQ(447, b.bottomAsUintNonInclusive());
}

TEST(Bounds, ContainsWorldPoint)
{
  // world corners
  EXPECT_EQ(false, b.containsWorldPoint(0, 0));
  EXPECT_EQ(false, b.containsWorldPoint(799, 0));
  EXPECT_EQ(false, b.containsWorldPoint(0, 599));
  EXPECT_EQ(false, b.containsWorldPoint(799, 599));

  // plate corners
  EXPECT_EQ(true, b.containsWorldPoint(10,   48));
  EXPECT_EQ(true, b.containsWorldPoint(509,  48));
  EXPECT_EQ(true, b.containsWorldPoint(10,  447));
  EXPECT_EQ(true, b.containsWorldPoint(509, 447));  

  // inside plate
  EXPECT_EQ(true, b.containsWorldPoint(10,  48));
  EXPECT_EQ(true, b.containsWorldPoint(120, 100));
  EXPECT_EQ(true, b.containsWorldPoint(400, 400));
  EXPECT_EQ(true, b.containsWorldPoint(509, 447));    

  // outside plate
  EXPECT_EQ(false, b.containsWorldPoint(10, 0));
  EXPECT_EQ(false, b.containsWorldPoint(10, 47));
  EXPECT_EQ(false, b.containsWorldPoint(10, 448));
  EXPECT_EQ(false, b.containsWorldPoint(10, 490));
  EXPECT_EQ(false, b.containsWorldPoint(100, 0));
  EXPECT_EQ(false, b.containsWorldPoint(100, 47));
  EXPECT_EQ(false, b.containsWorldPoint(100, 448));
  EXPECT_EQ(false, b.containsWorldPoint(100, 490));  
  EXPECT_EQ(false, b.containsWorldPoint(509, 0));
  EXPECT_EQ(false, b.containsWorldPoint(509, 47));
  EXPECT_EQ(false, b.containsWorldPoint(509, 448));
  EXPECT_EQ(false, b.containsWorldPoint(509, 490));    
}
