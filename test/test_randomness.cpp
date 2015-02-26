#include "platecapi.hpp"
#include "gtest/gtest.h"
#include <cstdlib>
#include "simplerandom.hpp"

using namespace std;

TEST(Randomness, SequenceDoubles)
{
  long seed = 3;
  SimpleRandom randsource(seed);

  EXPECT_FLOAT_EQ(5.1118433e-05, randsource.next_double());
  EXPECT_FLOAT_EQ(0.53070194, randsource.next_double());
  EXPECT_FLOAT_EQ(0.053402752, randsource.next_double());
}

TEST(Randomness, Maximum)
{
  long seed = 3;
  SimpleRandom randsource(seed);

  EXPECT_FLOAT_EQ(4.2949673e+09, randsource.maximum());
}
