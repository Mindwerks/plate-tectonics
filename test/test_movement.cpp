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

TEST(Movement, ApplyFriction)
{
	SimpleRandom sr(456);
	WorldDimension wd(50, 40);
	Movement mov(sr, wd);

	EXPECT_FLOAT_EQ(0.9989379f, mov.velX());
	EXPECT_FLOAT_EQ(0.046077024f, mov.velY());
	
	mov.applyFriction(2.2f, 10.5f);
	EXPECT_FLOAT_EQ(0.9989379f, mov.velX());
	EXPECT_FLOAT_EQ(0.046077024f, mov.velY());
	EXPECT_FLOAT_EQ(0.58095241f, mov.getVelocity());	

	mov.applyFriction(7.2f, 0.0f);
	EXPECT_FLOAT_EQ(0.0f, mov.getVelocity());
}

TEST(Movement, ApplyFrictionWithNullMass)
{
	SimpleRandom sr(456);
	WorldDimension wd(50, 40);
	Movement mov(sr, wd);

	mov.applyFriction(7.2f, 0.0f);
	EXPECT_FLOAT_EQ(0.0f, mov.getVelocity());
}

TEST(Movement, Move)
{
	SimpleRandom sr(789890);
	WorldDimension wd(500, 400);
	Movement mov(sr, wd);

	EXPECT_FLOAT_EQ(-0.29389676f, mov.velX());
	EXPECT_FLOAT_EQ(-0.95583719f, mov.velY());
	EXPECT_FLOAT_EQ(1.0f, mov.getVelocity());

	mov.move();
	EXPECT_FLOAT_EQ(-0.28745356f, mov.velX());
	EXPECT_FLOAT_EQ(-0.95779467f, mov.velY());
	EXPECT_FLOAT_EQ(1.0f, mov.getVelocity());	
}

TEST(Movement, VelocityOnXNoParams)
{
	SimpleRandom sr(789890);
	WorldDimension wd(500, 400);
	Movement mov(sr, wd);

	EXPECT_FLOAT_EQ(-0.29389676f, mov.velX());
	EXPECT_FLOAT_EQ(1.0f, mov.getVelocity());
	EXPECT_FLOAT_EQ(-0.29389676f, mov.velocityOnX());	
}

TEST(Movement, VelocityOnYNoParams)
{
	SimpleRandom sr(789890);
	WorldDimension wd(500, 400);
	Movement mov(sr, wd);

	EXPECT_FLOAT_EQ(-0.95583719f, mov.velY());
	EXPECT_FLOAT_EQ(1.0f, mov.getVelocity());
	EXPECT_FLOAT_EQ(-0.95583719f, mov.velocityOnY());	
}

TEST(Movement, VelocityOnXOneParam)
{
	SimpleRandom sr(789890);
	WorldDimension wd(500, 400);
	Movement mov(sr, wd);

	EXPECT_FLOAT_EQ(-0.29389676f, mov.velX());
	EXPECT_FLOAT_EQ(1.0f, mov.getVelocity());
	EXPECT_FLOAT_EQ(-2.9389676f, mov.velocityOnX(10.0));	
}

TEST(Movement, VelocityOnYOneParam)
{
	SimpleRandom sr(789890);
	WorldDimension wd(500, 400);
	Movement mov(sr, wd);

	EXPECT_FLOAT_EQ(-0.95583719f, mov.velY());
	EXPECT_FLOAT_EQ(1.0f, mov.getVelocity());
	EXPECT_FLOAT_EQ(-9.5583719f, mov.velocityOnY(10.0));	
}

TEST(Movement, Dot)
{
	SimpleRandom sr(789890);
	WorldDimension wd(500, 400);
	Movement mov(sr, wd);

	EXPECT_FLOAT_EQ(-0.29389676f, mov.velX());
	EXPECT_FLOAT_EQ(-0.95583719f, mov.velY());
	EXPECT_FLOAT_EQ(-3.45530509f, mov.dot(2.0, 3.0));
}
