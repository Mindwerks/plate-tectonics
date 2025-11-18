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

#include "platecapi.hpp"
#include "gtest/gtest.h"
#include <cstdlib>
#include "simplerandom.hpp"
#include <math.h>

using namespace std;

/* We want the float to behave consistently across platforms */
TEST(Portability, FloatOps)
{
    float v = 123456.789012f;

    for (int i=0; i<2; i++) {
        v *= v + 812345.0123f;
    }
    EXPECT_FLOAT_EQ(1.3347527e+22f, v);

    for (int i=0; i<3; i++) {
        v *= v + 812345.0123f;
    }
    EXPECT_FLOAT_EQ(std::numeric_limits<float>::infinity(), v);

    for (int i=0; i<95; i++) {
        v *= v + 812345.0123f;
    }
    EXPECT_FLOAT_EQ(INFINITY, v);
}

/* We want the float to behave consistently across platforms */
TEST(Randomness, DoubleOps)
{
    double v = 123456.789012f;

    for (int i=0; i<2; i++) {
        v *= v + 812345.0123f;
    }
    EXPECT_DOUBLE_EQ(1.3347525239012724e+22, v);

    for (int i=0; i<3; i++) {
        v *= v + 812345.0123f;
    }
    EXPECT_DOUBLE_EQ(1.0074094163955063e+177, v);

    for (int i=0; i<95; i++) {
        v *= v + 812345.0123f;
    }
    EXPECT_DOUBLE_EQ(INFINITY, v);
}
