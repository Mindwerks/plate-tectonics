#include "platecapi.hpp"
#include "gtest/gtest.h"
#include <cstdlib>
#include "simplerandom.hpp"

using namespace std;

TEST(Randomness, Sequence)
{
  long seed = 3;
  SimpleRandom randsource(seed);

  EXPECT_FLOAT_EQ(2.3656589e+09,  randsource.next());
  EXPECT_FLOAT_EQ(3.0376106e+08,  randsource.next());
  EXPECT_FLOAT_EQ(3.0414717e+09,  randsource.next());
}

TEST(Randomness, Max)
{
  long seed = 3;
  SimpleRandom randsource(seed);

  EXPECT_FLOAT_EQ(4.2949673e+09,  randsource.maximum());
}

