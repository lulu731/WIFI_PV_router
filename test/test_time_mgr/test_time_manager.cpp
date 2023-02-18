#include "time_manager.h"
#include "ntp_time_itf.h"
#include "solar_events_itf.h"
#include <ArduinoFake.h>
//#include <single_header/standalone/fakeit.hpp>
#include <unity.h>

using namespace fakeit;

time_t Now = 1676306813, SunRise = 1676306820, SunSet = 1676306830;

Mock<TIME_CLIENT_ITF> Time_Client_Mock;
TIME_CLIENT_ITF* tc;

Mock<SUN_EVENTS_ITF> Solar_Events_Mock;
SUN_EVENTS_ITF* se;

TIME_MGR* tm;

void setUp(void)
{
  ArduinoFakeReset();
  Method(Time_Client_Mock, Init) = true;
  When(Method(Time_Client_Mock, GetEpochTime)).Return(Now);
  tc = &Time_Client_Mock.get();
  se = &Solar_Events_Mock.get();

  When(Method(Solar_Events_Mock, GetNextEvents)).Do([](const time_t& aTime, time_t& aSunrise, time_t& aSunset) {
    aSunrise = SunRise;
    aSunset = SunSet;
  });
  tm = new TIME_MGR(*tc, *se);
}

void tearDown(void)
{
  delete tm;
}

void test_get_epochtime()
{
  Method(Time_Client_Mock, GetEpochTime) = Now;
  auto aEpochTime = tm->GetTime();
  TEST_ASSERT_EQUAL(Now, aEpochTime);
}

void test_handle_time_go_to_sleep()
{
/* Initialise tm => Sunrise et sunset fixés.
Check régulièrement si time est > sunset
Si oui, calcule next solar events et envoie go to sleep au serial
Check si time > sunrise, réveille Arduino*/
  // SunSet and SunRise are fixed
  When(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).AlwaysReturn();
  for (size_t Seconds = 0; Seconds < 18; Seconds++)
  {
    When(Method(Time_Client_Mock, GetEpochTime)).AlwaysReturn(Now + Seconds);

    tm->HandleTime();
  };
  TEST_ASSERT_EQUAL(SunSet + 1, Now + 18);

  When(Method(Time_Client_Mock, GetEpochTime)).AlwaysReturn(Now + 17);
  TEST_ASSERT_EQUAL(Now + 17, tm->GetTime());
  Verify(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).Once();
}

int main(int argc, char *argv[])
{
  UNITY_BEGIN();
  RUN_TEST(test_get_epochtime);
  RUN_TEST(test_handle_time_go_to_sleep);
  UNITY_END();

  return 0;
}