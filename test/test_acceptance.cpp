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

TEST(PlatecCreate, SameResultAsPlatec)
{
  long seed = 3;
  void* p = platec_api_create(seed, 512, 512, 0.65,60,0.02,1000000,0.33,2,10);
  const float* heightmap = platec_api_get_heightmap(p);

  EXPECT_FLOAT_EQ(1.6303145f,  heightmap[0]);
  EXPECT_FLOAT_EQ(1.5651948f,  heightmap[100]);
  EXPECT_FLOAT_EQ(0.1f, heightmap[200]);
  EXPECT_FLOAT_EQ(0.1f,   heightmap[1000]);
  EXPECT_FLOAT_EQ(0.1f,  heightmap[5000]);
  EXPECT_FLOAT_EQ(0.1f, heightmap[50000]);
  EXPECT_FLOAT_EQ(0.1f, heightmap[100000]);
  EXPECT_FLOAT_EQ(1.5906698f, heightmap[150000]);
  EXPECT_FLOAT_EQ(1.7254744f,  heightmap[200000]);
  EXPECT_FLOAT_EQ(0.1f, heightmap[250000]);
  EXPECT_FLOAT_EQ(1.6749345f,  heightmap[262143]);
}

TEST(PlatecGlobalGeneration, SameResultAsPlatec)
{
  long seed = 3;
  void* p = platec_api_create(seed, 512,512, 0.65,60,0.02,1000000,0.33,2,10);
  while (platec_api_is_finished(p) == 0) {
    platec_api_step(p);
  }
  const float* heightmap = platec_api_get_heightmap(p);

  EXPECT_FLOAT_EQ(1.1350174f, heightmap[0]);
  EXPECT_FLOAT_EQ(1.0035408f, heightmap[100]);
  EXPECT_FLOAT_EQ(1.2325575f, heightmap[200]);
  EXPECT_FLOAT_EQ(1.7264656f,  heightmap[1000]);
  EXPECT_FLOAT_EQ(0.098315857f, heightmap[5000]);
  EXPECT_FLOAT_EQ(0.093855403f, heightmap[50000]);
  EXPECT_FLOAT_EQ(0.25f,  heightmap[100000]);
  EXPECT_FLOAT_EQ(0.1f,  heightmap[150000]);
  EXPECT_FLOAT_EQ(0.10590123f, heightmap[200000]);
  EXPECT_FLOAT_EQ(1.3252633f, heightmap[250000]);
  EXPECT_FLOAT_EQ(1.1439542f, heightmap[262143]);
}
