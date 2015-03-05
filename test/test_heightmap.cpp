#include "heightmap.hpp"
#include "gtest/gtest.h"

TEST(HeightMap, SetAndGet)
{
  HeightMap hm = HeightMap(50, 20);
  hm.set( 0,  0, 0.2f);
  hm.set(20, 18, 0.7f);
  hm.set(40, 18, 0.5f);
  hm.set(49, 19, 0.9f);
  ASSERT_TRUE(0.2f == hm.get( 0,  0));
  ASSERT_TRUE(0.7f == hm.get(20, 18));
  ASSERT_TRUE(0.5f == hm.get(40, 18));
  ASSERT_TRUE(0.9f == hm.get(49, 19));
}

TEST(HeightMap, CopyConstructor)
{
  HeightMap hm = HeightMap(50, 20);
  hm.set( 0,  0, 0.2f);
  hm.set(20, 18, 0.7f);
  hm.set(40, 18, 0.5f);
  hm.set(49, 19, 0.9f);
  HeightMap hm2 = hm;
  ASSERT_TRUE(0.2f == hm2.get( 0,  0));
  ASSERT_TRUE(0.7f == hm2.get(20, 18));
  ASSERT_TRUE(0.5f == hm2.get(40, 18));
  ASSERT_TRUE(0.9f == hm2.get(49, 19));
}

TEST(HeightMap, AssignmentOperator)
{
  HeightMap hm = HeightMap(50, 20);
  hm.set( 0,  0, 0.2f);
  hm.set(20, 18, 0.7f);
  hm.set(40, 18, 0.5f);
  hm.set(49, 19, 0.9f);
  HeightMap hm2 = HeightMap(10, 10);
  hm2 = hm;
  ASSERT_TRUE(0.2f == hm2.get( 0,  0));
  ASSERT_TRUE(0.7f == hm2.get(20, 18));
  ASSERT_TRUE(0.5f == hm2.get(40, 18));
  ASSERT_TRUE(0.9f == hm2.get(49, 19));
}

TEST(HeightMap, SetAll)
{
  HeightMap hm = HeightMap(50, 20);
  hm.set_all( 1.789f );
  ASSERT_TRUE(1.789f == hm.get( 0,  0));
  ASSERT_TRUE(1.789f == hm.get(20, 18));
  ASSERT_TRUE(1.789f == hm.get(40, 18));
  ASSERT_TRUE(1.789f == hm.get(49, 19));
}

TEST(HeightMap, IndexedAccessOperator)
{
  HeightMap hm = HeightMap(50, 20);
  hm.set( 0,  0, 0.2f);
  hm.set(20, 18, 0.7f);
  hm.set(40, 18, 0.5f);
  hm.set(49, 19, 0.9f);

  ASSERT_TRUE(0.2f == hm[0]);
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
}

