#include "plate.hpp"
#include "gtest/gtest.h"
#include "rectangle.hpp"

using namespace Platec;

TEST(CreatePlate, SquareDoesNotExplode)
{
  const float *heightmap = new float[40000]; // 200 x 200
  plate p = plate(123, heightmap, 100, 3, 50, 23, 18, WorldDimension(200, 200));
}

TEST(CreatePlate, NotSquareDoesNotExplode)
{
  const float *heightmap = new float[80000]; // 200 x 400
  plate p = plate(123, heightmap, 100, 3, 50, 23, 18, WorldDimension(200, 400));
}

TEST(Plate, contains)
{
    const float *heightmap = new float[256 * 128];
    plate p = plate(123, heightmap, 100, 3, 50, 23, 18, WorldDimension(256, 128));
    EXPECT_EQ(true, p.contains(148, 25));
}

TEST(Plate, getBounds)
{
    const WorldDimension wd = WorldDimension(256, 128);
    const float *heightmap = new float[wd.getArea()];
    plate p = plate(123, heightmap, 
        100, 80, // width, height
        50, 23, // x, y
        18, // plate age
        wd);
    Rectangle expected = Rectangle(wd, UINT32_C(50), UINT32_C(150), UINT32_C(23), UINT32_C(103));
    EXPECT_EQ(expected, p.getBounds());  
}

// TODO test also when plate is large as world
TEST(Plate, calculateCrust)
{
  const float *heightmap = new float[256 * 128];
  plate p = plate(123, heightmap, 100, 3, 50, 23, 18, WorldDimension(256, 128));    
  uint32_t x, y, index;
  float w_crust, e_crust, n_crust, s_crust;
  uint32_t w, e, n, s;

  // top left corner
  x = 0;
  y = 0;
  index = 1;
  p.calculateCrust(x,y,index,w_crust,e_crust,n_crust,s_crust,w,e,n,s);
  EXPECT_EQ(0, w);
  EXPECT_EQ(1, e);
  EXPECT_EQ(0, n);
  EXPECT_EQ(100, s);

  // bottom right corner
  x = 99;
  y = 2;
  index = 1;
  p.calculateCrust(x,y,index,w_crust,e_crust,n_crust,s_crust,w,e,n,s);
  EXPECT_EQ(298, w);
  EXPECT_EQ(200, e);
  EXPECT_EQ(199, n);
  EXPECT_EQ(99, s);

  // point in the middle
  x = 50;
  y = 1;
  index = 1;
  p.calculateCrust(x,y,index,w_crust,e_crust,n_crust,s_crust,w,e,n,s);
  EXPECT_EQ(149, w);
  EXPECT_EQ(151, e);
  EXPECT_EQ(50, n);
  EXPECT_EQ(250, s); 
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}