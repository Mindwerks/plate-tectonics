#include "platecapi.hpp"
#include "gtest/gtest.h"
#include <cstdlib>

///
/// These sort of acceptance tests are derived by running platec
/// (the original library). We want to refactor the code while
/// obtaining the same results.
///

TEST(PlatecCreate, SameResultAsPlatec)
{
  long seed = 3;
  void* p = platec_api_create(seed, 512,0.65,60,0.02,1000000,0.33,2,10);
  const float* heightmap = platec_api_get_heightmap(p);

  EXPECT_FLOAT_EQ(1.6716905f,  heightmap[0]);
  EXPECT_FLOAT_EQ(1.6792216f,  heightmap[100]);
  EXPECT_FLOAT_EQ(0.1f, heightmap[200]);
  EXPECT_FLOAT_EQ(1.6712158f,   heightmap[1000]);
  EXPECT_FLOAT_EQ(1.7788768f,  heightmap[5000]);
  EXPECT_FLOAT_EQ(1.7062505f, heightmap[50000]);
  EXPECT_FLOAT_EQ(0.1f, heightmap[100000]);
  EXPECT_FLOAT_EQ(1.6885694f, heightmap[150000]);
  EXPECT_FLOAT_EQ(1.7441362f,  heightmap[200000]);
  EXPECT_FLOAT_EQ(1.6631076f, heightmap[250000]);
  EXPECT_FLOAT_EQ(1.7181f,  heightmap[262143]);
}

TEST(PlatecGlobalGeneration, SameResultAsPlatec)
{
  long seed = 3;
  void* p = platec_api_create(seed, 512,0.65,60,0.02,1000000,0.33,2,10);
  while (platec_api_is_finished(p) == 0) {
    platec_api_step(p);
  }
  const float* heightmap = platec_api_get_heightmap(p);

  EXPECT_FLOAT_EQ(1.3747238f, heightmap[0]);
  EXPECT_FLOAT_EQ(2.650918f, heightmap[100]);
  EXPECT_FLOAT_EQ(1.1874427f, heightmap[200]);
  EXPECT_FLOAT_EQ(2.7036309f,  heightmap[1000]);
  EXPECT_FLOAT_EQ(11.566978f, heightmap[5000]);
  EXPECT_FLOAT_EQ(3.5252557f, heightmap[50000]);
  EXPECT_FLOAT_EQ(6.2049809f,  heightmap[100000]);
  EXPECT_FLOAT_EQ(0.076983266f,  heightmap[150000]);
  EXPECT_FLOAT_EQ(0.076983273f, heightmap[200000]);
  EXPECT_FLOAT_EQ(1.1638799f, heightmap[250000]);
  EXPECT_FLOAT_EQ(3.7328248f, heightmap[262143]);
}
