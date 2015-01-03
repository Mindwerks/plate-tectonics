#include "platecapi.hpp"
#include "gtest/gtest.h"
#include <cstdlib>
#include <random>

using namespace std;

TEST(Randomness, Sequence)
{
  long seed = 3;
  mt19937 randsource(seed);

  EXPECT_FLOAT_EQ(2.3656589e+09,  randsource());
  EXPECT_FLOAT_EQ(3.0376106e+08,  randsource());
  EXPECT_FLOAT_EQ(3.0414717e+09,  randsource());
}

