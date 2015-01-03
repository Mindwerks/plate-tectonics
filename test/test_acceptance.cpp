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

  EXPECT_FLOAT_EQ(1.8901190757751465f,  heightmap[0]);
  EXPECT_FLOAT_EQ(1.9333564043045044f,  heightmap[100]);
  EXPECT_FLOAT_EQ(0.10000000149011612f, heightmap[200]);
  EXPECT_FLOAT_EQ(1.785782814025879f,   heightmap[1000]);
  EXPECT_FLOAT_EQ(1.8174481391906738f,  heightmap[5000]);
  EXPECT_FLOAT_EQ(0.10000000149011612f, heightmap[50000]);
  EXPECT_FLOAT_EQ(0.10000000149011612f, heightmap[100000]);
  EXPECT_FLOAT_EQ(0.10000000149011612f, heightmap[150000]);
  EXPECT_FLOAT_EQ(1.7049407958984375f,  heightmap[200000]);
  EXPECT_FLOAT_EQ(0.10000000149011612f, heightmap[250000]);
  EXPECT_FLOAT_EQ(1.7803807258605957f,  heightmap[262143]);
}

TEST(PlatecGlobalGeneration, SameResultAsPlatec)
{
  long seed = 3;
  void* p = platec_api_create(seed, 512,0.65,60,0.02,1000000,0.33,2,10);
  while (platec_api_is_finished(p) == 0) {
    platec_api_step(p);
  }
  const float* heightmap = platec_api_get_heightmap(p);

  EXPECT_FLOAT_EQ(0.13348780572414398f, heightmap[0]);
  EXPECT_FLOAT_EQ(0.21046529710292816f, heightmap[100]);
  EXPECT_FLOAT_EQ(0.17170283198356628f, heightmap[200]);
  EXPECT_FLOAT_EQ(0.1930341273546219f,  heightmap[1000]);
  EXPECT_FLOAT_EQ(0.14793574810028076f, heightmap[5000]);
  EXPECT_FLOAT_EQ(0.15966156125068665f, heightmap[50000]);
  EXPECT_FLOAT_EQ(2.9937195777893066f,  heightmap[100000]);
  EXPECT_FLOAT_EQ(0.1959685981273651f,  heightmap[150000]);
  EXPECT_FLOAT_EQ(0.18909034132957458f, heightmap[200000]);
  EXPECT_FLOAT_EQ(0.18724516034126282f, heightmap[250000]);
  EXPECT_FLOAT_EQ(0.17201107740402222f, heightmap[262143]);
}
