#include "platecapi.hpp"
#include "gtest/gtest.h"
#include <cstdlib>
#include "simplerandom.hpp"
#include <math.h>

using namespace std;

/* We want the float to behave consistently across platforms */
TEST(Portability, FloatOps)
{
  float v = 123456.789012f;

  for (int i=0; i<2; i++) {
  	v *= v + 812345.0123f;
  }
  EXPECT_FLOAT_EQ(1.3347527e+22, v);  

  for (int i=0; i<3; i++) {
  	v *= v + 812345.0123f;
  }
  EXPECT_FLOAT_EQ(INFINITY, v);

  for (int i=0; i<95; i++) {
  	v *= v + 812345.0123f;
  }
  EXPECT_FLOAT_EQ(INFINITY, v);
}

/* We want the float to behave consistently across platforms */
TEST(Randomness, DoubleOps)
{
  double v = 123456.789012f;

  for (int i=0; i<2; i++) {
  	v *= v + 812345.0123f;
  }
  EXPECT_DOUBLE_EQ(1.3347525239012724e+22, v);

  for (int i=0; i<3; i++) {
  	v *= v + 812345.0123f;
  }
  EXPECT_DOUBLE_EQ(1.0074094163955063e+177, v);

  for (int i=0; i<95; i++) {
  	v *= v + 812345.0123f;
  }
  EXPECT_DOUBLE_EQ(INFINITY, v);
}
