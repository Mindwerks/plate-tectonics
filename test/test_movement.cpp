#include "movement.hpp"
#include "mass.hpp"
#include "plate.hpp"
#include "gtest/gtest.h"

using namespace Platec;

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

class MockPlate : public IPlate {
public:
	MockPlate(const FloatVector& velocityUnitVector, float mass, const FloatPoint& massCenter) 
		: _velocityUnitVector(velocityUnitVector),
		  _mass(mass),
		  _massCenter(massCenter),
		  _decImpulseDelta(NULL)
	{ }

	~MockPlate() {
		if (_decImpulseDelta) delete _decImpulseDelta;
	}

	FloatVector velocityUnitVector() const {
		return _velocityUnitVector;
	}

	void decImpulse(const FloatVector& delta) {
		_decImpulseDelta = new FloatVector(delta);
	}

	FloatVector decImpulseDelta() {
		if (_decImpulseDelta == NULL) throw runtime_error("(MockPlate::decImpulseDelta) Data not ready");
		return *_decImpulseDelta;
	}

	float getMass() const {
		return _mass;
	}

	FloatPoint massCenter() const {
		return _massCenter;
	}

private:
	FloatVector _velocityUnitVector;
	FloatVector* _decImpulseDelta;
	float _mass;
	FloatPoint _massCenter;
};

TEST(Movement, Collide)
{
	SimpleRandom sr(789890);
	WorldDimension wd(500, 400);
	Movement mov(sr, wd);

	EXPECT_FLOAT_EQ(-0.29389676f, mov.velX());
	EXPECT_FLOAT_EQ(-0.95583719f, mov.velY());
	EXPECT_FLOAT_EQ(1.0f, mov.getVelocity());

	Mass thisMass(100.0, 70.0, 90.0);
	FloatVector otherPlateVelocityUnitVector(0.0f, -1.0f);
	float otherPlateMass = 10000.0f;
	FloatPoint otherPlateMassCenter(100.0f, 400.0f);
	MockPlate otherPlate(otherPlateVelocityUnitVector, otherPlateMass, otherPlateMassCenter);
	mov.collide(thisMass, otherPlate, 356, 439, 456.2f);

	EXPECT_FLOAT_EQ(-6.2893254e-05, otherPlate.decImpulseDelta().x());
	EXPECT_FLOAT_EQ(-0.00064989703f, otherPlate.decImpulseDelta().y());
}
