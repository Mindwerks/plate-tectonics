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

#include "heightmap.hpp"
#include "gtest/gtest.h"

TEST(HeightMap, ConstructorWidthHeight) {
    HeightMap hm = HeightMap(50, 20);
    ASSERT_EQ(50, hm.width());
    ASSERT_EQ(20, hm.height());
}

TEST(HeightMap, Area) {
    HeightMap hm = HeightMap(50, 20);
    ASSERT_EQ(1000, hm.area());
}

TEST(HeightMap, SetAndGet) {
    HeightMap hm = HeightMap(50, 20);
    hm.set(Platec::Point2D<uint32_t>(0, 0), 0.2f);
    hm.set(Platec::Point2D<uint32_t>(20, 18), 0.7f);
    hm.set(Platec::Point2D<uint32_t>(40, 18), 0.5f);
    hm.set(Platec::Point2D<uint32_t>(49, 19), 0.9f);
    ASSERT_FLOAT_EQ(0.2f , hm.get(Platec::Point2D<uint32_t>(0,  0)));
    ASSERT_FLOAT_EQ(0.7f , hm.get(Platec::Point2D<uint32_t>(20, 18)));
    ASSERT_FLOAT_EQ(0.5f , hm.get(Platec::Point2D<uint32_t>(40, 18)));
    ASSERT_FLOAT_EQ(0.9f , hm.get(Platec::Point2D<uint32_t>(49, 19)));
}

TEST(HeightMap, CopyConstructor) {
    HeightMap hm = HeightMap(50, 20);
    hm.set(Platec::Point2D<uint32_t>(0, 0), 0.2f);
    hm.set(Platec::Point2D<uint32_t>(20, 18), 0.7f);
    hm.set(Platec::Point2D<uint32_t>(40, 18), 0.5f);
    hm.set(Platec::Point2D<uint32_t>(49, 19), 0.9f);
    HeightMap hm2 = hm;
    ASSERT_FLOAT_EQ(0.2f , hm2.get(Platec::Point2D<uint32_t>(0, 0)));
    ASSERT_FLOAT_EQ(0.7f , hm2.get(Platec::Point2D<uint32_t>(20, 18)));
    ASSERT_FLOAT_EQ(0.5f , hm2.get(Platec::Point2D<uint32_t>(40, 18)));
    ASSERT_FLOAT_EQ(0.9f , hm2.get(Platec::Point2D<uint32_t>(49, 19)));
}

TEST(HeightMap, AssignmentOperator) {
    HeightMap hm = HeightMap(50, 20);
    hm.set(Platec::Point2D<uint32_t>(0, 0), 0.2f);
    hm.set(Platec::Point2D<uint32_t>(20, 18), 0.7f);
    hm.set(Platec::Point2D<uint32_t>(40, 18), 0.5f);
    hm.set(Platec::Point2D<uint32_t>(49, 19), 0.9f);
    HeightMap hm2 = HeightMap(10, 10);
    hm2 = hm;
    ASSERT_FLOAT_EQ(0.2f , hm2.get(Platec::Point2D<uint32_t>(0, 0)));
    ASSERT_FLOAT_EQ(0.7f , hm2.get(Platec::Point2D<uint32_t>(20, 18)));
    ASSERT_FLOAT_EQ(0.5f , hm2.get(Platec::Point2D<uint32_t>(40, 18)));
    ASSERT_FLOAT_EQ(0.9f , hm2.get(Platec::Point2D<uint32_t>(49, 19)));
}

TEST(HeightMap, SetAll) {
    HeightMap hm = HeightMap(50, 20);
    hm.set_all(1.789f);
    ASSERT_FLOAT_EQ(1.789f , hm.get(Platec::Point2D<uint32_t>(0, 0)));
    ASSERT_FLOAT_EQ(1.789f , hm.get(Platec::Point2D<uint32_t>(20, 18)));
    ASSERT_FLOAT_EQ(1.789f , hm.get(Platec::Point2D<uint32_t>(40, 18)));
    ASSERT_FLOAT_EQ(1.789f , hm.get(Platec::Point2D<uint32_t>(49, 19)));
}

TEST(HeightMap, IndexedAccessOperatorFromIndex) {
    HeightMap hm = HeightMap(50, 20);
    hm.set(Platec::Point2D<uint32_t>(0, 0), 0.2f);
    hm.set(Platec::Point2D<uint32_t>(20, 18), 0.7f);
    hm.set(Platec::Point2D<uint32_t>(40, 18), 0.5f);
    hm.set(Platec::Point2D<uint32_t>(49, 19), 0.9f);

    ASSERT_FLOAT_EQ(0.2f , hm[0]);
    ASSERT_FLOAT_EQ(0.7f , hm[920]);
    ASSERT_FLOAT_EQ(0.5f , hm[940]);
    ASSERT_FLOAT_EQ(0.9f , hm[999]);

    hm[0]   += 0.1f;
    hm[920] += 0.1f;
    hm[940] -= 0.1f;
    hm[999] -= 0.1f;

    EXPECT_FLOAT_EQ(0.3f, hm[0]);
    EXPECT_FLOAT_EQ(0.8f, hm[920]);
    EXPECT_FLOAT_EQ(0.4f, hm[940]);
    EXPECT_FLOAT_EQ(0.8f, hm[999]);
}

/*TEST(HeightMap, IndexedAccessOperatorFromWorldPoint)
{
  HeightMap hm = HeightMap(50, 20);
  hm.set( 0,  0, 0.2f);
  hm.set(20, 18, 0.7f);
  hm.set(40, 18, 0.5f);
  hm.set(49, 19, 0.9f);

  ASSERT_TRUE(0.2f == hm[WorldPoint()]);
  ASSERT_TRUE(0.7f == hm[920]);
  ASSERT_TRUE(0.5f == hm[940]);
  ASSERT_TRUE(0.9f == hm[999]);

  hm[0]   += 0.1;
  hm[920] += 0.1;
  hm[940] -= 0.1;
  hm[999] -= 0.1;

  EXPECT_FLOAT_EQ(0.3f, hm[0]);
  EXPECT_FLOAT_EQ(0.8f, hm[920]);
  EXPECT_FLOAT_EQ(0.4f, hm[940]);
  EXPECT_FLOAT_EQ(0.8f, hm[999]);
}*/
