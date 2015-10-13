/******************************************************************************
 *  plate-tectonics, a plate tectonics simulation library
 *  Copyright (C) 2012-2013 Lauri Viitanen
 *  Copyright (C) 2014-2015 Federico Tomassetti, Bret Curtis
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, see http://www.gnu.org/licenses/
 *****************************************************************************/

#include "bounds.hpp"
#include "gtest/gtest.h"

const WorldDimension wd(800, 600);
const FloatPoint topLeft(10.2f, 48.9f);
const Dimension plateDim(500, 400);
const Bounds b(wd, topLeft, plateDim);

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

TEST(Bounds, IsInLimits)
{
    // negative coordinates
    EXPECT_EQ(false, b.isInLimits(-1, 10));
    EXPECT_EQ(false, b.isInLimits(10, -1));
    EXPECT_EQ(false, b.isInLimits(-1, -1));

    EXPECT_EQ(true, b.isInLimits(0, 0));
    EXPECT_EQ(true, b.isInLimits(124.3f, 245.56f));
    EXPECT_EQ(true, b.isInLimits(499, 399));
    EXPECT_EQ(true, b.isInLimits(499.1f, 399.1f));
    EXPECT_EQ(true, b.isInLimits(499.999f, 399.999f));

    EXPECT_EQ(false, b.isInLimits(500, 399));
    EXPECT_EQ(false, b.isInLimits(499, 400));
    EXPECT_EQ(false, b.isInLimits(500, 400));
}

TEST(Bounds, Shift)
{
    Bounds bounds(wd, topLeft, plateDim);
    // topLeft = 10.2, 48.9
    bounds.shift(10.7f, 100.1f);
    // now topLeft should be = 20.9, 149.0
    EXPECT_EQ(20, bounds.leftAsUint());
    EXPECT_EQ(149, bounds.topAsUint());
    // width and heigh should not be affected
    EXPECT_EQ(500, bounds.width());
    EXPECT_EQ(400, bounds.height());
}

TEST(Bounds, Grow)
{
    Bounds bounds(wd, topLeft, plateDim);
    bounds.grow(123, 0);

    EXPECT_EQ(623, bounds.width());
    // height should not be affected
    EXPECT_EQ(400, bounds.height());
    // topLeft not be affected
    EXPECT_EQ(10, bounds.leftAsUint());
    EXPECT_EQ(48, bounds.topAsUint());

    Bounds bounds2(wd, topLeft, plateDim);
    bounds2.grow(0, 123);

    EXPECT_EQ(523, bounds2.height());
    // width should not be affected
    EXPECT_EQ(500, bounds2.width());
    // topLeft not be affected
    EXPECT_EQ(10, bounds2.leftAsUint());
    EXPECT_EQ(48, bounds2.topAsUint());
}

TEST(Bounds, GetMapIndex)
{
    uint32_t px, py, res;

    px = 10;
    py = 48;
    res = b.getMapIndex(&px, &py);
    ASSERT_EQ(px, 0);
    ASSERT_EQ(py, 0);
    ASSERT_EQ(res, 0);

    px = 510;
    py =  48;
    res = b.getMapIndex(&px, &py);
    ASSERT_EQ(px, 510);
    ASSERT_EQ(py, 48);
    ASSERT_EQ(res, BAD_INDEX);

    px = 10;
    py = 448;
    res = b.getMapIndex(&px, &py);
    ASSERT_EQ(px, 10);
    ASSERT_EQ(py, 448);
    ASSERT_EQ(res, BAD_INDEX);

    px = 110;
    py = 98;
    res = b.getMapIndex(&px, &py);
    ASSERT_EQ(px, 100);
    ASSERT_EQ(py, 50);
    ASSERT_EQ(res, 25100);
}

TEST(Bounds, GetValidMapIndex)
{
    uint32_t px, py, res;

    px = 10;
    py = 48;
    res = b.getValidMapIndex(&px, &py);
    ASSERT_EQ(px, 0);
    ASSERT_EQ(py, 0);
    ASSERT_EQ(res, 0);

    px = 110;
    py = 98;
    res = b.getValidMapIndex(&px, &py);
    ASSERT_EQ(px, 100);
    ASSERT_EQ(py, 50);
    ASSERT_EQ(res, 25100);
}


