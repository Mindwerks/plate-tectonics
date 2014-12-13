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

TEST(HeightMap, CopyRawToNoAllocate)
{
    HeightMap hm = HeightMap(30, 10);
    hm.set( 3, 8, 0.123);
    hm.set(23, 4, 0.234);
    hm.set(12, 9, 0.345);

    float* dst = new float[300];
    hm.copy_raw_to(dst);

    EXPECT_FLOAT_EQ(0.123f, hm.get( 3, 8));
    EXPECT_FLOAT_EQ(0.234f, hm.get(23, 4));
    EXPECT_FLOAT_EQ(0.345f, hm.get(12, 9));
}

TEST(HeightMap, CopyRawToAllocate)
{
    HeightMap hm = HeightMap(30, 10);
    hm.set( 3, 8, 0.123);
    hm.set(23, 4, 0.234);
    hm.set(12, 9, 0.345);

    float* dst = NULL;
    hm.copy_raw_to(dst, true);

    EXPECT_FLOAT_EQ(0.123f, hm.get( 3, 8));
    EXPECT_FLOAT_EQ(0.234f, hm.get(23, 4));
    EXPECT_FLOAT_EQ(0.345f, hm.get(12, 9));
}

TEST(HeightMap, From)
{
    float* src = new float[300];
    src[0]   = 0.01f;
    src[100] = 0.02f;
    src[200] = 0.03f;
    src[299] = 0.04f;

    HeightMap hm = HeightMap(30, 10);
    hm.from(src);

    EXPECT_FLOAT_EQ(0.01f, hm.get( 0,  0));
    EXPECT_FLOAT_EQ(0.02f, hm.get(10,  3));
    EXPECT_FLOAT_EQ(0.03f, hm.get(20,  6));
    EXPECT_FLOAT_EQ(0.04f, hm.get(29,  9));
}
