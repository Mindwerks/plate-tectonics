#include "plate.hpp"
#include "gtest/gtest.h"

TEST(CreatePlate)
{
  const float *heightmap = new float[40000]; // 200 x 200
  plate p = plate(heightmap, 100, 3, 50, 23, 18, 200);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}