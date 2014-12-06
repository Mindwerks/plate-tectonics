// test.cpp
  #include <UnitTest++/UnitTest++.h>

  TEST(FailSpectacularly)
  {
    CHECK(false);
  }

  int main()
  {
    return UnitTest::RunAllTests();
  }