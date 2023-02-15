#include "time_manager.h"
#include "ntp_time_itf.h"
#include <ArduinoFake.h>
#include <unity.h>

using namespace fakeit;

Mock<TIME_CLIENT_ITF> Time_Client_Mock;

void setUp(void)
{
  // Nothing to do.
}


void tearDown(void)
{
  // Nothing to do.
}


void test_get_epochtime()
{
  Method(Time_Client_Mock, Init) = true;
  Method(Time_Client_Mock, GetEpochTime) = 1676306813;

  TIME_CLIENT_ITF &tc = Time_Client_Mock.get();
  TIME_MGR tm(tc);
  auto aEpochTime = tm.GetTime();

  Verify(Method(Time_Client_Mock, Init));
  Verify(Method(Time_Client_Mock, GetEpochTime));

  TEST_ASSERT_EQUAL(1676306813, aEpochTime);
}


int main(int argc, char *argv[])
{
  UNITY_BEGIN();
  RUN_TEST(test_get_epochtime);
  UNITY_END();

  return 0;
}