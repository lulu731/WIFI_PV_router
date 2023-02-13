#include <unity.h>

void setUp(void)
{
  //SUN_EVENT SunEvent;
}


void tearDown(void)
{
  // Nothing to do.
}

void test_main()
{
  TEST_FAIL();
}

int main(int argc, char *argv[])
{
  UNITY_BEGIN();
  RUN_TEST(test_main);
  UNITY_END();
}