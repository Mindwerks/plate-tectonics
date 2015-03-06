#include "movement.hpp"
#include "gtest/gtest.h"

TEST(Movement, Constructor)
{
	SimpleRandom sr(123);
	WorldDimension wd(5, 4);
	Movement mov(sr, wd);
	EXPECT_FLOAT_EQ(0.99992257f, mov.velX());
	EXPECT_FLOAT_EQ(0.01244594f, mov.velY());
	EXPECT_FLOAT_EQ(1.0f, mov.getVelocity());
}
