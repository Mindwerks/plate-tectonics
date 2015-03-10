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

using namespace std;

TEST(Randomness, SequenceDoubles)
{
  long seed = 3;
  SimpleRandom randsource(seed);

  EXPECT_FLOAT_EQ(5.1118433e-05, randsource.next_double());
  EXPECT_FLOAT_EQ(0.53070194, randsource.next_double());
  EXPECT_FLOAT_EQ(0.053402752, randsource.next_double());
}

TEST(Randomness, Maximum)
{
  long seed = 3;
  SimpleRandom randsource(seed);

  EXPECT_FLOAT_EQ(4.2949673e+09, randsource.maximum());
}
