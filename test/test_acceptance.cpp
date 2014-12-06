#include "platecapi.hpp"
#include <UnitTest++/UnitTest++.h>
#include <cstdlib>

///
/// These sort of acceptance tests are derived by running platec
/// (the original library). We want to refactor the code while
/// obtaining the same results.
///

TEST(PlatecCreate)
{
  srand(3);
  void* p = platec_api_create(512,0.65,60,0.02,1000000,0.33,2,10);
  const float* heightmap = platec_api_get_heightmap(p);

  CHECK(1.8901190757751465f == heightmap[0]);
  CHECK(1.9333564043045044f == heightmap[100]);
  CHECK(0.10000000149011612f == heightmap[200]);
  CHECK(0.10000000149011612f == heightmap[100000]);
  CHECK(1.7803807258605957f == heightmap[262143]);
}

TEST(PlatecGlobalGeneration)
{
  srand(3);
  void* p = platec_api_create(512,0.65,60,0.02,1000000,0.33,2,10);
  while (platec_api_is_finished(p) == 0) {
    platec_api_step(p);
  }
  const float* heightmap = platec_api_get_heightmap(p);

  CHECK(0.13348780572414398f == heightmap[0]);
  CHECK(0.21046529710292816f == heightmap[100]);
  CHECK(0.17170283198356628f == heightmap[200]);
  CHECK(2.9937195777893066f  == heightmap[100000]);
  CHECK(0.17201107740402222f == heightmap[262143]);
}
