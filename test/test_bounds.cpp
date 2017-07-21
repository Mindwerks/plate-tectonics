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
#include "world_properties.h"

const Dimension wd(800, 600);
const Platec::vec2f topLeft(10.2f, 48.9f);
const Platec::vec2f topLeftWrapping(700.4f, 500.3);
const Dimension plateDim(500, 400);
const Bounds b( topLeft, plateDim);
const Bounds b2(topLeftWrapping, plateDim);


TEST(Bounds, Index) {
    world_properties::get().setWorldDimension(wd);
    EXPECT_EQ(0u, b.index(Platec::vec2ui(0, 0)));
    EXPECT_EQ(100100u, b.index(Platec::vec2ui(100, 200)));
    EXPECT_EQ(199999u, b.index(Platec::vec2ui(499, 399)));
}

TEST(Bounds, Area) {
    EXPECT_EQ(200000u, b.area());
}

TEST(Bounds, Width) {
    EXPECT_EQ(500u, b.width());
}

TEST(Bounds, Height) {
    EXPECT_EQ(400u, b.height());
}

TEST(Bounds, LeftAsUint) {
    EXPECT_EQ(10u, b.left());
}

TEST(Bounds, TopAsUint) {
    EXPECT_EQ(48u, b.top());
}

TEST(Bounds, RightAsUintNonInclusive) {
    EXPECT_EQ(509u, b.rightAsUintNonInclusive());
}

TEST(Bounds, BottomAsUintNonInclusive) {
    EXPECT_EQ(447u, b.bottomAsUintNonInclusive());
}

TEST(Bounds, ContainsWorldPoint) {
    // world corners
    EXPECT_EQ(false, b.containsWorldPoint(Platec::vec2ui(0, 0)));
    EXPECT_EQ(false, b.containsWorldPoint(Platec::vec2ui(799, 0)));
    EXPECT_EQ(false, b.containsWorldPoint(Platec::vec2ui(0, 599)));
    EXPECT_EQ(false, b.containsWorldPoint(Platec::vec2ui(799, 599)));

    // plate corners
    EXPECT_EQ(true, b.containsWorldPoint(Platec::vec2ui(10,   48)));
    EXPECT_EQ(true, b.containsWorldPoint(Platec::vec2ui(509,  48)));
    EXPECT_EQ(true, b.containsWorldPoint(Platec::vec2ui(10,  447)));
    EXPECT_EQ(true, b.containsWorldPoint(Platec::vec2ui(509, 447)));

    // inside plate
    EXPECT_EQ(true, b.containsWorldPoint(Platec::vec2ui(10,  48)));
    EXPECT_EQ(true, b.containsWorldPoint(Platec::vec2ui(120, 100)));
    EXPECT_EQ(true, b.containsWorldPoint(Platec::vec2ui(400, 400)));
    EXPECT_EQ(true, b.containsWorldPoint(Platec::vec2ui(509, 447)));

    // outside plate
    EXPECT_EQ(false, b.containsWorldPoint(Platec::vec2ui(10, 0)));
    EXPECT_EQ(false, b.containsWorldPoint(Platec::vec2ui(10, 47)));
    EXPECT_EQ(false, b.containsWorldPoint(Platec::vec2ui(10, 448)));
    EXPECT_EQ(false, b.containsWorldPoint(Platec::vec2ui(10, 490)));
    EXPECT_EQ(false, b.containsWorldPoint(Platec::vec2ui(100, 0)));
    EXPECT_EQ(false, b.containsWorldPoint(Platec::vec2ui(100, 47)));
    EXPECT_EQ(false, b.containsWorldPoint(Platec::vec2ui(100, 448)));
    EXPECT_EQ(false, b.containsWorldPoint(Platec::vec2ui(100, 490)));
    EXPECT_EQ(false, b.containsWorldPoint(Platec::vec2ui(509, 0)));
    EXPECT_EQ(false, b.containsWorldPoint(Platec::vec2ui(509, 47)));
    EXPECT_EQ(false, b.containsWorldPoint(Platec::vec2ui(509, 448)));
    EXPECT_EQ(false, b.containsWorldPoint(Platec::vec2ui(509, 490)));
}

TEST(Bounds, IsInLimits) {
    // negative coordinates
    EXPECT_EQ(false, b.isInLimits(Platec::vec2ui(-1, 10)));
    EXPECT_EQ(false, b.isInLimits(Platec::vec2ui(10, -1)));
    EXPECT_EQ(false, b.isInLimits(Platec::vec2ui(-1, -1)));

    EXPECT_EQ(true, b.isInLimits(Platec::vec2ui(0, 0)));
    EXPECT_EQ(true, b.isInLimits(Platec::vec2ui(124.3f, 245.56f)));
    EXPECT_EQ(true, b.isInLimits(Platec::vec2ui(499, 399)));
    EXPECT_EQ(true, b.isInLimits(Platec::vec2ui(499.1f, 399.1f)));
    EXPECT_EQ(true, b.isInLimits(Platec::vec2ui(499.999f, 399.999f)));

    EXPECT_EQ(false, b.isInLimits(Platec::vec2ui(500, 399)));
    EXPECT_EQ(false, b.isInLimits(Platec::vec2ui(499, 400)));
    EXPECT_EQ(false, b.isInLimits(Platec::vec2ui(500, 400)));
}

TEST(Bounds, Shift) {
    Bounds bounds(topLeft, plateDim);
    // topLeft = 10.2, 48.9
    bounds.shift(Platec::Vector2D<float_t>(10.7f, 100.1f));
    // now topLeft should be = 20.9, 149.0
    EXPECT_EQ(20u, bounds.left());
    EXPECT_EQ(149u, bounds.top());
    // width and heigh should not be affected
    EXPECT_EQ(500u, bounds.width());
    EXPECT_EQ(400u, bounds.height());
}

TEST(Bounds, Grow) {
    Bounds bounds(topLeft, plateDim);
    bounds.grow(Platec::Vector2D<uint32_t>(123, 0));

    EXPECT_EQ(623u, bounds.width());
    // height should not be affected
    EXPECT_EQ(400u, bounds.height());
    // topLeft not be affected
    EXPECT_EQ(10u, bounds.left());
    EXPECT_EQ(48u, bounds.top());

    Bounds bounds2(topLeft, plateDim);
    bounds2.grow(Platec::Vector2D<uint32_t>(0, 123));

    EXPECT_EQ(523u, bounds2.height());
    // width should not be affected
    EXPECT_EQ(500u, bounds2.width());
    // topLeft not be affected
    EXPECT_EQ(10u, bounds2.left());
    EXPECT_EQ(48u, bounds2.top());
}

TEST(Bounds, GetMapIndex) {
    uint32_t px, py;

    px = 10;
    py = 48;
    auto res = b.getMapIndex(Platec::vec2ui(px, py));
    ASSERT_EQ(res.second.x(), 0u);
    ASSERT_EQ(res.second.y(), 0u);
    ASSERT_EQ(res.first, 0u);

    px = 510;
    py =  48;
    res = b.getMapIndex(Platec::vec2ui(px, py));
    ASSERT_EQ(res.second.x(), 510u);
    ASSERT_EQ(res.second.y(), 48u);
    ASSERT_EQ(res.first, BAD_INDEX);

    px = 10;
    py = 448;
    res = b.getMapIndex(Platec::vec2ui(px, py));
    ASSERT_EQ(res.second.x(), 10u);
    ASSERT_EQ(res.second.y(), 448u);
    ASSERT_EQ(res.first, BAD_INDEX);

    px = 110;
    py = 98;
    res = b.getMapIndex(Platec::vec2ui(px, py));
    ASSERT_EQ(res.second.x(), 100u);
    ASSERT_EQ(res.second.y(), 50u);
    ASSERT_EQ(res.first, 25100u);

    px = 750;
    py = 550;
    res = b2.getMapIndex(Platec::vec2ui(px, py));
    ASSERT_EQ(res.second.x(), 50u);
    ASSERT_EQ(res.second.y(), 50u);
    ASSERT_EQ(res.first, 25050u);

    px = 0;
    py = 0;
    res = b2.getMapIndex(Platec::vec2ui(px, py));
    ASSERT_EQ(res.second.x(), 100u);
    ASSERT_EQ(res.second.y(), 100u);
    ASSERT_EQ(res.first, 50100u);

    px = 0;
    py = 600;
    res = b2.getMapIndex(Platec::vec2ui(px, py));
    ASSERT_EQ(res.second.x(), 100u);
    ASSERT_EQ(res.second.y(), 100u);
    ASSERT_EQ(res.first, 50100u);

    px = 800;
    py = 0;
    res = b2.getMapIndex(Platec::vec2ui(px, py));
    ASSERT_EQ(res.second.x(), 100u);
    ASSERT_EQ(res.second.y(), 100u);
    ASSERT_EQ(res.first, 50100u);

    px = 500;
    py = 200;
    res = b2.getMapIndex(Platec::vec2ui(px, py));
    ASSERT_EQ(res.second.x(), 500u);
    ASSERT_EQ(res.second.y(), 200u);
    ASSERT_EQ(res.first, BAD_INDEX);
}

TEST(Bounds, GetValidMapIndex) {
    uint32_t px, py;

    px = 10;
    py = 48;
    auto res = b.getValidMapIndex(Platec::vec2ui(px, py));
    ASSERT_EQ(res.second.x(), 0u);
    ASSERT_EQ(res.second.y(), 0u);
    ASSERT_EQ(res.first, 0u);

    px = 110;
    py = 98;
    res = b.getValidMapIndex(Platec::vec2ui(px, py));
    ASSERT_EQ(res.second.x(), 100u);
    ASSERT_EQ(res.second.y(), 50u);
    ASSERT_EQ(res.first, 25100u);
}


