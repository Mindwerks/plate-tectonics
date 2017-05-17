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

#include "rectangle.hpp"
#include <cstdio>
#include "gtest/gtest.h"

using Platec::Rectangle;

TEST(Rectangle, MapIndexInsideRectNotWrapping)
{
    Platec::Rectangle r = Platec::Rectangle(Dimension(50, 30), 42, 48, 8, 15);
    uint32_t px, py, res;

    px = 42;
    py =  8;
    res = r.getMapIndex(&px, &py);
    ASSERT_EQ(px, 0);
    ASSERT_EQ(py, 0);
    ASSERT_EQ(res, 0);

    px = 43;
    py =  8;
    res = r.getMapIndex(&px, &py);
    ASSERT_EQ(px, 1);
    ASSERT_EQ(py, 0);
    ASSERT_EQ(res, 1);

    px = 42;
    py =  9;
    res = r.getMapIndex(&px, &py);
    ASSERT_EQ(px, 0);
    ASSERT_EQ(py, 1);
    ASSERT_EQ(res, 6);
}

TEST(Rectangle, MapIndexOutsideRect)
{
    Platec::Rectangle r = Platec::Rectangle(Dimension(50, 30), 42, 48, 8, 15);
    uint32_t px, py, res;

    px = 49;
    py =  8;
    res = r.getMapIndex(&px, &py);
    ASSERT_EQ(px, 49);
    ASSERT_EQ(py, 8);
    ASSERT_EQ(res, -1);

    px = 48;
    py =  15;
    res = r.getMapIndex(&px, &py);
    ASSERT_EQ(px, 48);
    ASSERT_EQ(py, 15);
    ASSERT_EQ(res, -1);

    px = 2;
    py = 2;
    res = r.getMapIndex(&px, &py);
    ASSERT_EQ(px, 2);
    ASSERT_EQ(py, 2);
    ASSERT_EQ(res, -1);
}

TEST(Rectangle, MapIndexInsideRectWrappingOnX)
{
    Platec::Rectangle r = Platec::Rectangle(Dimension(50, 30), 42, 6, 8, 12);
    uint32_t px, py, res;

    px = 42;
    py =  8;
    res = r.getMapIndex(&px, &py);
    ASSERT_EQ(px, 0);
    ASSERT_EQ(py, 0);
    ASSERT_EQ(res, 0);

    px =  0;
    py =  8;
    res = r.getMapIndex(&px, &py);
    ASSERT_EQ(px, 8);
    ASSERT_EQ(py, 0);
    ASSERT_EQ(res, 8);

    px =  2;
    py =  9;
    res = r.getMapIndex(&px, &py);
    ASSERT_EQ(px, 10);
    ASSERT_EQ(py,  1);
    ASSERT_EQ(res, 24);
}

TEST(Rectangle, MapIndexInsideRectWrappingOnY)
{
    Platec::Rectangle r = Platec::Rectangle(Dimension(50, 30), 42, 48, 25, 5);
    uint32_t px, py, res;

    px = 42;
    py = 25;
    res = r.getMapIndex(&px, &py);
    ASSERT_EQ(px, 0);
    ASSERT_EQ(py, 0);
    ASSERT_EQ(res, 0);

    px =  44;
    py =  29;
    res = r.getMapIndex(&px, &py);
    ASSERT_EQ(px, 2);
    ASSERT_EQ(py, 4);
    ASSERT_EQ(res, 26);

    px =  44;
    py =   2;
    res = r.getMapIndex(&px, &py);
    ASSERT_EQ(px, 2);
    ASSERT_EQ(py, 7);
    ASSERT_EQ(res, 44);
}

TEST(Rectangle, MapIndexInsideRectLargeAs)
{
    Platec::Rectangle r = Platec::Rectangle(Dimension(50, 30), 0, 50, 0, 30);
    uint32_t px, py, res;

    px = 0;
    py = 0;
    res = r.getMapIndex(&px, &py);
    ASSERT_EQ(px, 0);
    ASSERT_EQ(py, 0);
    ASSERT_EQ(res, 0);

    px =  12;
    py =  8;
    res = r.getMapIndex(&px, &py);
    ASSERT_EQ(px, 12);
    ASSERT_EQ(py, 8);
    ASSERT_EQ(res, 412);

    px =  49;
    py =  29;
    res = r.getMapIndex(&px, &py);
    ASSERT_EQ(px, 49);
    ASSERT_EQ(py, 29);
    ASSERT_EQ(res, 1499);
}
