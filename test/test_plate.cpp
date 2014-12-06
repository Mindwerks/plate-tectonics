#include "plate.hpp"
#include <UnitTest++/UnitTest++.h>

TEST(CreatePlate)
{
  const float *heightmap = new float[40000]; // 200 x 200
  plate p = plate(heightmap, 100, 3, 50, 23, 18, 200);
}

int main()
{
return UnitTest::RunAllTests();
}