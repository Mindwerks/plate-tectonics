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
  /*long seed = 3;
  void* p = platec_api_create(seed, 512,0.65,60,0.02,1000000,0.33,2,10);
  const float* heightmap = platec_api_get_heightmap(p);

  EXPECT_FLOAT_EQ(0.1f,  heightmap[0]);
  EXPECT_FLOAT_EQ(0.1f,  heightmap[100]);
  EXPECT_FLOAT_EQ(1.5174572f, heightmap[200]);
  EXPECT_FLOAT_EQ(0.1f,   heightmap[1000]);
  EXPECT_FLOAT_EQ(0.1,  heightmap[5000]);
  EXPECT_FLOAT_EQ(0.10000000149011612f, heightmap[50000]);
  EXPECT_FLOAT_EQ(1.5513532f, heightmap[100000]);
  EXPECT_FLOAT_EQ(0.10000000149011612f, heightmap[150000]);
  EXPECT_FLOAT_EQ(1.7961829f,  heightmap[200000]);
  EXPECT_FLOAT_EQ(0.10000000149011612f, heightmap[250000]);
  EXPECT_FLOAT_EQ(0.1f,  heightmap[262143]);*/
}

TEST(PlatecGlobalGeneration, SameResultAsPlatec)
{
  /*long seed = 3;
  void* p = platec_api_create(seed, 512,0.65,60,0.02,1000000,0.33,2,10);
  while (platec_api_is_finished(p) == 0) {
    platec_api_step(p);
  }
  const float* heightmap = platec_api_get_heightmap(p);

  EXPECT_FLOAT_EQ(0.14913777f, heightmap[0]);
  EXPECT_FLOAT_EQ(0.23923723f, heightmap[100]);
  EXPECT_FLOAT_EQ(1.1308481f, heightmap[200]);
  EXPECT_FLOAT_EQ(0.18773384f,  heightmap[1000]);
  EXPECT_FLOAT_EQ(0.26646918f, heightmap[5000]);
  EXPECT_FLOAT_EQ(0.24811202f, heightmap[50000]);
  EXPECT_FLOAT_EQ(0.3424814f,  heightmap[100000]);
  EXPECT_FLOAT_EQ(0.23238662,  heightmap[150000]);
  EXPECT_FLOAT_EQ(0.23216549f, heightmap[200000]);
  EXPECT_FLOAT_EQ(1.4414167f, heightmap[250000]);
  EXPECT_FLOAT_EQ(0.14604542f, heightmap[262143]);*/
}
