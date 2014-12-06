#include "heightmap.hpp"
#include <UnitTest++/UnitTest++.h>

TEST(SetAndGetHeightMap)
{
  HeightMap hm = HeightMap(50, 20);
  hm.set(40, 18, 0.5f);
  CHECK(0.5f == hm.get(40, 18));
}
