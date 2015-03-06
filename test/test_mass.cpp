#include "mass.hpp"
#include "gtest/gtest.h"

TEST(MassBuilder, ConstructorFromHeightmap)
{
    float heightmap[20] = {0.0f, 0.0f, 0.0f, 0.0f, 10.3f,
                           5.0f, 0.0f, 0.0f, 0.0f,  0.0f,
                           0.2f, 0.0f, 0.0f, 0.0f,  0.0f,
                           1.0f, 1.0f, 0.0f, 0.0f,  0.0f};
    Dimension dim(5, 4);
    MassBuilder mb(heightmap, dim);
    EXPECT_FLOAT_EQ(17.5f, mb.build().getMass());
    EXPECT_FLOAT_EQ(2.4114285714285715f, mb.build().getCx());
    EXPECT_FLOAT_EQ(0.6514285714285715f, mb.build().getCy());
}
