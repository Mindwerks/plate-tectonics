#include "rectangle.hpp"
#include <cstdio>
#include "gtest/gtest.h"

#include "testing.hpp"

#define ASSERT_EQUAL(actual, expected) { if (expected != actual) fprintf(stderr, "Expected %i, actual %i\n", expected, actual); CHECK(expected == actual); }

TEST(RectangleMapIndexInsideRectNotWrapping)
{
    Rectangle r = Rectangle(50, 30, 42, 48, 8, 15);
    size_t px, py, res;

    px = 42;
    py =  8;
    res = r.getMapIndex(&px, &py);
    ASSERT_EQUAL(px, 0);
    ASSERT_EQUAL(py, 0);
    ASSERT_EQUAL(res, 0);

    px = 43;
    py =  8;
    res = r.getMapIndex(&px, &py);
    ASSERT_EQUAL(px, 1);
    ASSERT_EQUAL(py, 0);
    ASSERT_EQUAL(res, 1);

    px = 42;
    py =  9;
    res = r.getMapIndex(&px, &py);
    ASSERT_EQUAL(px, 0);
    ASSERT_EQUAL(py, 1);
    ASSERT_EQUAL(res, 6);
}

TEST(RectangleMapIndexOutsideRect)
{
    Rectangle r = Rectangle(50, 30, 42, 48, 8, 15);
    size_t px, py, res;

    px = 49;
    py =  8;
    res = r.getMapIndex(&px, &py);
    ASSERT_EQUAL(px, 49);
    ASSERT_EQUAL(py, 8);
    ASSERT_EQUAL(res, -1);

    px = 48;
    py =  15;
    res = r.getMapIndex(&px, &py);
    ASSERT_EQUAL(px, 48);
    ASSERT_EQUAL(py, 15);
    ASSERT_EQUAL(res, -1);

    px = 2;
    py = 2;
    res = r.getMapIndex(&px, &py);
    ASSERT_EQUAL(px, 2);
    ASSERT_EQUAL(py, 2);
    ASSERT_EQUAL(res, -1);
}

TEST(RectangleMapIndexInsideRectWrappingOnX)
{
    Rectangle r = Rectangle(50, 30, 42, 6, 8, 12);
    size_t px, py, res;

    px = 42;
    py =  8;
    res = r.getMapIndex(&px, &py);
    ASSERT_EQUAL(px, 0);
    ASSERT_EQUAL(py, 0);
    ASSERT_EQUAL(res, 0);

    px =  0;
    py =  8;
    res = r.getMapIndex(&px, &py);
    ASSERT_EQUAL(px, 8);
    ASSERT_EQUAL(py, 0);
    ASSERT_EQUAL(res, 8);

    px =  2;
    py =  9;
    res = r.getMapIndex(&px, &py);
    ASSERT_EQUAL(px, 10);
    ASSERT_EQUAL(py,  1);
    ASSERT_EQUAL(res, 24);
}

TEST(RectangleMapIndexInsideRectWrappingOnY)
{
    Rectangle r = Rectangle(50, 30, 42, 48, 25, 5);
    size_t px, py, res;

    px = 42;
    py = 25;
    res = r.getMapIndex(&px, &py);
    ASSERT_EQUAL(px, 0);
    ASSERT_EQUAL(py, 0);
    ASSERT_EQUAL(res, 0);

    px =  44;
    py =  29;
    res = r.getMapIndex(&px, &py);
    ASSERT_EQUAL(px, 2);
    ASSERT_EQUAL(py, 4);
    ASSERT_EQUAL(res, 26);

    px =  44;
    py =   2;
    res = r.getMapIndex(&px, &py);
    ASSERT_EQUAL(px, 2);
    ASSERT_EQUAL(py, 7);
    ASSERT_EQUAL(res, 44);
}

TEST(RectangleMapIndexInsideRectLargeAsWorld)
{
}
