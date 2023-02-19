#include "time_manager.h"
#include "ntp_time_itf.h"
#include "solar_events_itf.h"
#include <ArduinoFake.h>
#include <unity.h>

using namespace fakeit;

time_t Now = 1676306813;
const time_t SunRise = 1676306820, SunSet = 1676306830;

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

void PastSecondsFromNow(const time_t aSeconds)
{
  for (size_t Seconds = 1; Seconds < aSeconds + 1; Seconds++)
  {
    When(Method(Time_Client_Mock, GetEpochTime)).Return(Now + Seconds);

    tm->HandleTime();
  };
}

void PastSecondsFromNowWithFailure(const time_t aSeconds, const time_t aEventSecond)
{
  // Hardware failure at event, preventing the sleep command to be called
  time_t FailingSecond = aEventSecond - Now;
  for (size_t Seconds = 1; Seconds < aSeconds + 1; Seconds++)
  {
    When(Method(Time_Client_Mock, GetEpochTime)).Return(Now + Seconds);
    if (Seconds != FailingSecond)
      tm->HandleTime();
  };
}

time_t TimeAfterEvent(const int aSecondsToAdd, const time_t aEventTime)
{
  return aEventTime + aSecondsToAdd;
}


class TEST_HANDLE_TIME_SLEEP
{
public:
  static void AtSunsetShouldGoToSleep()
  {
    // SunSet and SunRise are initiated in Time_Manager constructor
    When(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).AlwaysReturn();
    PastSecondsFromNow(SunSet - Now);
    Verify(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).Once();
  }

  // Failure case: SunSet time is reached, but hardware failure is not able to call sleep command
  static void AtSunsetHardwareFailureShouldGoToSleep()
  {
    When(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).AlwaysReturn();
    PastSecondsFromNowWithFailure(TimeAfterEvent(5, SunSet) - Now, SunSet);
    Verify(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).Once();
  }

  static void OverSunsetShouldCallSleepOnce()
  {
    When(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).AlwaysReturn();
    PastSecondsFromNow(TimeAfterEvent(5, SunSet) - Now);
    Verify(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).Once();
  }
};

int main(int argc, char *argv[])
{
  UNITY_BEGIN();
  RUN_TEST(test_get_epochtime);
  RUN_TEST(TEST_HANDLE_TIME_SLEEP::AtSunsetShouldGoToSleep);
  RUN_TEST(TEST_HANDLE_TIME_SLEEP::OverSunsetShouldCallSleepOnce);
  RUN_TEST(TEST_HANDLE_TIME_SLEEP::AtSunsetHardwareFailureShouldGoToSleep);
  UNITY_END();

  return 0;
}