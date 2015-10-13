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

///
/// These sort of acceptance tests were originallu derived by running platec
/// (the original library). We want to refactor the code while
/// obtaining the same results.
///
/// Unfortunately they are still platform dependant.
/// They work on Linux (and especially on the Travis servers).
/// In the future we will work on having the same results on all
/// the platforms, for now are used as guards against undesired changes.
///

/*TEST(PlatecCreate, SameResultAsPlatec)
{
  long seed = 3;
  void* p = platec_api_create(seed, 512, 512, 0.65,60,0.02,1000000,0.33,2,10);
  const float* heightmap = platec_api_get_heightmap(p);

  EXPECT_FLOAT_EQ(0.1,  heightmap[0]);
  EXPECT_FLOAT_EQ(1.483476f,  heightmap[100]);
  EXPECT_FLOAT_EQ(0.1f, heightmap[200]);
  EXPECT_FLOAT_EQ(1.5217931f,   heightmap[1000]);
  EXPECT_FLOAT_EQ(0.1f,  heightmap[5000]);
  EXPECT_FLOAT_EQ(1.4538962f, heightmap[50000]);
  EXPECT_FLOAT_EQ(1.5340517f, heightmap[100000]);
  EXPECT_FLOAT_EQ(0.1f, heightmap[150000]);
  EXPECT_FLOAT_EQ(1.492384f,  heightmap[200000]);
  EXPECT_FLOAT_EQ(0.1f, heightmap[250000]);
  EXPECT_FLOAT_EQ(0.1f, heightmap[262143]);
}

TEST(PlatecGlobalGeneration, SameResultAsPlatec)
{
  long seed = 3;
  void* p = platec_api_create(seed, 512,512, 0.65,60,0.02,1000000,0.33,2,10);
  while (platec_api_is_finished(p) == 0) {
    platec_api_step(p);
  }
  const float* heightmap = platec_api_get_heightmap(p);

  EXPECT_FLOAT_EQ(2.0832214f, heightmap[0]);
  EXPECT_FLOAT_EQ(0.089928061f, heightmap[100]);
  EXPECT_FLOAT_EQ(0.092105672f, heightmap[200]);
  EXPECT_FLOAT_EQ(0.10110986f,  heightmap[1000]);
  EXPECT_FLOAT_EQ(0.10199541f, heightmap[5000]);
  EXPECT_FLOAT_EQ(0.11336569f, heightmap[50000]);
  EXPECT_FLOAT_EQ(1.0184617f,  heightmap[100000]);
  EXPECT_FLOAT_EQ(0.84557754f,  heightmap[150000]);
  EXPECT_FLOAT_EQ(0.12073194f, heightmap[200000]);
  EXPECT_FLOAT_EQ(0.12088145f, heightmap[250000]);
  EXPECT_FLOAT_EQ(2.1296265f, heightmap[262143]);
}*/
