/******************************************************************************
 *  plate-tectonics, a plate tectonics simulation library
 *  Copyright (C) 2012-2013 Lauri Viitanen
 *  Copyright (C) 2014-2015 Federico Tomassetti, Bret Curtis
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, see http://www.gnu.org/licenses/
 *****************************************************************************/

#include "movement.hpp"
#include "mass.hpp"
#include "plate.hpp"
#include "gtest/gtest.h"

using namespace Platec;

TEST(Movement, Constructor)
{
    SimpleRandom sr(123);
    Dimension wd(5, 4);
    Movement mov(sr);
    EXPECT_FLOAT_EQ(0.99992257f, mov.velocityUnitVector().x());
    EXPECT_FLOAT_EQ(0.01244594f, mov.velocityUnitVector().y());
    EXPECT_FLOAT_EQ(1.0f, mov.getVelocity());
}

TEST(Movement, ApplyFriction)
{
    SimpleRandom sr(456);
    Dimension wd(50, 40);
    Movement mov(sr);

    EXPECT_FLOAT_EQ(0.9989379f, mov.velocityUnitVector().x());
    EXPECT_FLOAT_EQ(0.046077024f, mov.velocityUnitVector().y());

    mov.applyFriction(2.2f, 10.5f);
    EXPECT_FLOAT_EQ(0.9989379f, mov.velocityUnitVector().x());
    EXPECT_FLOAT_EQ(0.046077024f, mov.velocityUnitVector().y());
    EXPECT_FLOAT_EQ(0.58095241f, mov.getVelocity());

    mov.applyFriction(7.2f, 0.0f);
    EXPECT_FLOAT_EQ(0.0f, mov.getVelocity());
}

TEST(Movement, ApplyFrictionWithNullMass)
{
    SimpleRandom sr(456);
    Dimension wd(50, 40);
    Movement mov(sr);

    mov.applyFriction(7.2f, 0.0f);
    EXPECT_FLOAT_EQ(0.0f, mov.getVelocity());
}

TEST(Movement, Move)
{
    SimpleRandom sr(789890);
    Dimension wd(500, 400);
    Movement mov(sr);

    EXPECT_FLOAT_EQ(-0.29389676f, mov.velocityUnitVector().x());
    EXPECT_FLOAT_EQ(-0.95583719f, mov.velocityUnitVector().y());
    EXPECT_FLOAT_EQ(1.0f, mov.getVelocity());

    mov.move(wd);
    EXPECT_FLOAT_EQ(-0.28745356f, mov.velocityUnitVector().x());
    EXPECT_FLOAT_EQ(-0.95779467f, mov.velocityUnitVector().y());
    EXPECT_FLOAT_EQ(1.0f, mov.getVelocity());
}

TEST(Movement, VelocityOnXNoParams)
{
    SimpleRandom sr(789890);
    Dimension wd(500, 400);
    Movement mov(sr);

    EXPECT_FLOAT_EQ(-0.29389676f, mov.velocityUnitVector().x());
    EXPECT_FLOAT_EQ(1.0f, mov.getVelocity());
    EXPECT_FLOAT_EQ(-0.29389676f, mov.velocityUnitVector().x());
}

TEST(Movement, VelocityOnYNoParams)
{
    SimpleRandom sr(789890);
    Dimension wd(500, 400);
    Movement mov(sr);

    EXPECT_FLOAT_EQ(-0.95583719f, mov.velocityUnitVector().y());
    EXPECT_FLOAT_EQ(1.0f, mov.getVelocity());
    EXPECT_FLOAT_EQ(-0.95583719f, mov.velocityVector().y());
}

TEST(Movement, VelocityOnXOneParam)
{
    SimpleRandom sr(789890);
    Dimension wd(500, 400);
    Movement mov(sr);

    EXPECT_FLOAT_EQ(-0.29389676f, mov.velocityUnitVector().x());
    EXPECT_FLOAT_EQ(1.0f, mov.getVelocity());
    EXPECT_FLOAT_EQ(-2.9389676f, mov.velocityOn(10.0).x());
}

TEST(Movement, VelocityOnYOneParam)
{
    SimpleRandom sr(789890);
    Dimension wd(500, 400);
    Movement mov(sr);

    EXPECT_FLOAT_EQ(-0.95583719f, mov.velocityUnitVector().y());
    EXPECT_FLOAT_EQ(1.0f, mov.getVelocity());
    EXPECT_FLOAT_EQ(-9.5583719f, mov.velocityOn(10.0).y());
}

TEST(Movement, Dot)
{
    SimpleRandom sr(789890);
    Dimension wd(500, 400);
    Movement mov(sr);

    EXPECT_FLOAT_EQ(-0.29389676f, mov.velocityUnitVector().x());
    EXPECT_FLOAT_EQ(-0.95583719f, mov.velocityUnitVector().y());
    EXPECT_FLOAT_EQ(-3.45530509f, mov.dot(Platec::vec2f(2.f, 3.f)));
}

class MockPlate : public IPlate {
public:
    MockPlate(const Platec::vec2f& velocityUnitVector, float mass, const Platec::vec2f & massCenter)
        : _velocityUnitVector(velocityUnitVector),
          _mass(mass),
          _massCenter(massCenter)
       { }

    Platec::vec2f velocityUnitVector() const {
        return _velocityUnitVector;
    }

    void decImpulse(const Platec::vec2f& delta) {
        _decImpulseDelta = Platec::vec2f(delta);
    }

    Platec::vec2f decImpulseDelta() {
        return _decImpulseDelta;
    }

    float getMass() const {
        return _mass;
    }

    const Platec::vec2f massCenter() const {
        return _massCenter;
    }

private:
    Platec::vec2f _velocityUnitVector;
    Platec::vec2f _decImpulseDelta = Platec::vec2f( 0.f, 0.f);
    float _mass;
    Platec::vec2f  _massCenter;
};

TEST(Movement, Collide)
{
    SimpleRandom sr(789890);
    Dimension wd(500, 400);
    Movement mov(sr);

    EXPECT_FLOAT_EQ(-0.29389676f, mov.velocityVector().x());
    EXPECT_FLOAT_EQ(-0.95583719f, mov.velocityVector().y());
    EXPECT_FLOAT_EQ(1.0f, mov.getVelocity());

    Mass thisMass(100.0, Platec::vec2f(70.0, 90.0));
    Platec::vec2f otherPlateVelocityUnitVector(0.0f, -1.0f);
    float otherPlateMass = 10000.0f;
    Platec::vec2f  otherPlateMassCenter(100.0f, 400.0f);
    MockPlate otherPlate(otherPlateVelocityUnitVector, otherPlateMass, otherPlateMassCenter);
    mov.collide(thisMass, otherPlate, 456.2f);

    EXPECT_FLOAT_EQ(-6.2893458e-05, otherPlate.decImpulseDelta().x());
    EXPECT_FLOAT_EQ(-0.00064989907f, otherPlate.decImpulseDelta().y());
}
