#include "time_manager.h"
#include "ntp_time_itf.h"
#include "solar_events_itf.h"
#include <ArduinoFake.h>
#include <unity.h>

using namespace fakeit;

time_t Now = 1676306813;
const time_t Sunrise = 1676306820, Sunset = Sunrise + 10;
const time_t NextSunrise = Sunset + 30, NextSunset = NextSunrise + 10;

Mock<TIME_CLIENT_ITF> Time_Client_Mock;
TIME_CLIENT_ITF* tc;

Mock<SUN_EVENTS_ITF> Solar_Events_Mock;
SUN_EVENTS_ITF* se;

TIME_MGR* tm;

std::array<time_t, 3> Args;
void WhenCallMockGetNextEvents(const time_t& aNewTime,const time_t& aNewSunrise, const time_t& aNewSunset)
{
  When(Method(Solar_Events_Mock, GetNextEvents)).Do([&](const time_t& aTime, time_t& aSunrise, time_t& aSunset) {
    aSunrise = aNewSunrise;
    aSunset = aNewSunset;
    Args[0] = aNewTime;;
    Args[1] = aNewSunrise;
    Args[2] = aNewSunset;
  });
}

void setUp(void)
{
  ArduinoFakeReset();
  Method(Time_Client_Mock, Init) = true;
  When(Method(Time_Client_Mock, GetEpochTime)).Return(Now);
  tc = &Time_Client_Mock.get();
  se = &Solar_Events_Mock.get();

  WhenCallMockGetNextEvents(Now, Sunrise, Sunset);
  tm = new TIME_MGR(*tc, *se);
  Solar_Events_Mock.ClearInvocationHistory();
  Time_Client_Mock.ClearInvocationHistory();
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

void HandleTimeFromNowToEvent(const time_t aEvent)
{
  for (size_t Seconds = 1; Seconds < aEvent - Now + 1; Seconds++)
  {
    When(Method(Time_Client_Mock, GetEpochTime)).AlwaysReturn(Now + Seconds);
    tm->HandleTime();
  };
}

void HandleTimeFromNowWithFailureAtEvent(const time_t aSeconds, const time_t aEventSecond)
{
  // Hardware failure at event, preventing the sleep command to be called
  time_t FailingSecond = aEventSecond - Now;
  for (size_t Seconds = 1; Seconds < aSeconds + 1; Seconds++)
  {
    When(Method(Time_Client_Mock, GetEpochTime)).AlwaysReturn(Now + Seconds);
    if (Seconds != FailingSecond)
      tm->HandleTime();
  };
}


class TEST_HANDLE_TIME_SLEEP
{
public:
  static void AtSunsetShouldGoToSleep()
  {
    // Sunset and Sunrise are initiated in Time_Manager constructor
    When(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).AlwaysReturn();
    WhenCallMockGetNextEvents(Now, NextSunrise, NextSunset);
    HandleTimeFromNowToEvent(Sunset);
    Verify(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).Once();
  }

  static void AtSunsetShouldGetNewSolarEvents()
  {
    When(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).AlwaysReturn();
    WhenCallMockGetNextEvents(Now, NextSunrise, NextSunset);
    HandleTimeFromNowToEvent(Sunset);
    std::array<time_t, 3> ExpectedArgs = {Now, NextSunrise, NextSunset};
    TEST_ASSERT_EQUAL_INT32_ARRAY(ExpectedArgs.data(), Args.data(), 3);
    Verify(Method(Solar_Events_Mock, GetNextEvents) +
      OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).Once();
  }

  // Failure case: Sunset time is reached, but hardware failure is not able to call sleep command
  static void AtSunsetHardwareFailureShouldGoToSleep()
  {
    When(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).AlwaysReturn();
    WhenCallMockGetNextEvents(Now, NextSunrise, NextSunset);
    HandleTimeFromNowWithFailureAtEvent(Sunset + 2 - Now, Sunset);
    Verify(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).Once();
  }

  static void OverSunsetShouldCallSleepOnce()
  {
    When(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).AlwaysReturn();
    WhenCallMockGetNextEvents(Now, NextSunrise, NextSunset);
    HandleTimeFromNowToEvent(Sunset + 5);
    Verify(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).Once();
  }
};


class TEST_HANDLE_TIME_WAKEUP
{
public:
  static void AtSunriseShouldWakeup()
  {
    When(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).AlwaysReturn();
    When(Method(ArduinoFake(), digitalWrite).Using(12, HIGH)).AlwaysReturn();
    WhenCallMockGetNextEvents(Now, NextSunrise, NextSunset);
    HandleTimeFromNowToEvent(NextSunrise);
    Verify(Method(ArduinoFake(), digitalWrite));
  }
};


int main(int argc, char *argv[])
{
  UNITY_BEGIN();
  RUN_TEST(test_get_epochtime);

  RUN_TEST(TEST_HANDLE_TIME_SLEEP::AtSunsetShouldGoToSleep);
  RUN_TEST(TEST_HANDLE_TIME_SLEEP::AtSunsetShouldGetNewSolarEvents);
  RUN_TEST(TEST_HANDLE_TIME_SLEEP::AtSunsetHardwareFailureShouldGoToSleep);
  RUN_TEST(TEST_HANDLE_TIME_SLEEP::OverSunsetShouldCallSleepOnce);

  RUN_TEST(TEST_HANDLE_TIME_WAKEUP::AtSunriseShouldWakeup);

  return UNITY_END();
}