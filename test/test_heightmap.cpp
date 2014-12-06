#include "heightmap.hpp"
#include <UnitTest++/UnitTest++.h>

TEST(HeightMapSetAndGet)
{
  HeightMap hm = HeightMap(50, 20);
  hm.set( 0,  0, 0.2f);
  hm.set(20, 18, 0.7f);
  hm.set(40, 18, 0.5f);
  hm.set(49, 19, 0.9f);
  CHECK(0.2f == hm.get( 0,  0));
  CHECK(0.7f == hm.get(20, 18));
  CHECK(0.5f == hm.get(40, 18));
  CHECK(0.9f == hm.get(49, 19));
}

TEST(HeightMapCopyConstructor)
{
  HeightMap hm = HeightMap(50, 20);
  hm.set( 0,  0, 0.2f);
  hm.set(20, 18, 0.7f);
  hm.set(40, 18, 0.5f);
  hm.set(49, 19, 0.9f);
  HeightMap hm2 = hm;
  CHECK(0.2f == hm2.get( 0,  0));
  CHECK(0.7f == hm2.get(20, 18));
  CHECK(0.5f == hm2.get(40, 18));
  CHECK(0.9f == hm2.get(49, 19));
}

TEST(HeightMapAssignmentOperator)
{
  HeightMap hm = HeightMap(50, 20);
  hm.set( 0,  0, 0.2f);
  hm.set(20, 18, 0.7f);
  hm.set(40, 18, 0.5f);
  hm.set(49, 19, 0.9f);
  HeightMap hm2 = HeightMap(10, 10);
  hm2 = hm;
  CHECK(0.2f == hm2.get( 0,  0));
  CHECK(0.7f == hm2.get(20, 18));
  CHECK(0.5f == hm2.get(40, 18));
  CHECK(0.9f == hm2.get(49, 19));
}

TEST(HeightMapSetAll)
{
  HeightMap hm = HeightMap(50, 20);
  hm.set_all( 1.789f );
  CHECK(1.789f == hm.get( 0,  0));
  CHECK(1.789f == hm.get(20, 18));
  CHECK(1.789f == hm.get(40, 18));
  CHECK(1.789f == hm.get(49, 19));
}
