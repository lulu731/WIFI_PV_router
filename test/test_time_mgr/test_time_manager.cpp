#include "time_manager.h"
#include "ntp_time_itf.h"
#include "solar_events_itf.h"
#include <ArduinoFake.h>
#include <unity.h>

using namespace fakeit;

const int Daylight = 10, DayDuration = 40, Night = 30;
time_t Now = 1676306813;
const time_t Sunrise = 1676306820, Sunset = Sunrise + Daylight;

Mock<TIME_CLIENT_ITF> Time_Client_Mock;
TIME_CLIENT_ITF* tc;

Mock<SUN_EVENTS_ITF> Solar_Events_Mock;
SUN_EVENTS_ITF* se;

TIME_MGR* tm;

std::array<time_t, 3> Args;
void WhenCallMockGetNextEvents()
{
  When(Method(Solar_Events_Mock, GetNextEvents)).AlwaysDo([&](const time_t& aTime, time_t& aSunrise, time_t& aSunset)
  {
    if (aTime == Now)
    {
      aSunrise = Sunrise;
      aSunset = Sunset;
    }
    else
    {
      aSunrise = aTime + Night;
      aSunset = aTime + DayDuration;
      Args[0] = aTime;
      Args[1] = aSunrise;
      Args[2] = aSunset;
    }
  });
}

void setUp(void)
{
  ArduinoFakeReset();
  Method(Time_Client_Mock, Init) = true;
  When(Method(Time_Client_Mock, GetEpochTime)).Return(Now);
  tc = &Time_Client_Mock.get();
  se = &Solar_Events_Mock.get();

  WhenCallMockGetNextEvents();
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


class TEST_HANDLE_TIME
{
public:
  static void AtSunsetShouldGoToSleep()
  {
    // Sunset and Sunrise are initiated in Time_Manager constructor
    When(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).AlwaysReturn();
    When(Method(ArduinoFake(), digitalWrite).Using(12, LOW)).AlwaysReturn();
    WhenCallMockGetNextEvents();
    HandleTimeFromNowToEvent(Sunset);
    Verify(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).Once();
  }

  static void AtSunsetShouldGetNewSolarEvents()
  {
    When(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).AlwaysReturn();
    When(Method(ArduinoFake(), digitalWrite).Using(12, LOW)).AlwaysReturn();
    WhenCallMockGetNextEvents();
    HandleTimeFromNowToEvent(Sunset);
    std::array<time_t, 3> ExpectedArgs = {Sunset, Sunset + Night, Sunset + DayDuration};
    TEST_ASSERT_EQUAL_INT32_ARRAY(ExpectedArgs.data(), Args.data(), 3);
    Verify(Method(Solar_Events_Mock, GetNextEvents) +
      OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).Once();
  }

  // Failure case: Sunset time is reached, but hardware failure is not able to call sleep command
  static void AtSunsetHardwareFailureShouldGoToSleep()
  {
    When(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).AlwaysReturn();
    When(Method(ArduinoFake(), digitalWrite).Using(12, LOW)).AlwaysReturn();
    WhenCallMockGetNextEvents();
    HandleTimeFromNowWithFailureAtEvent(Sunset + 2 - Now, Sunset);
    Verify(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).Once();
  }

  static void OverSunsetShouldCallSleepOnce()
  {
    When(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).AlwaysReturn();
    When(Method(ArduinoFake(), digitalWrite).Using(12, LOW)).AlwaysReturn();
    WhenCallMockGetNextEvents();
    HandleTimeFromNowToEvent(Sunset + 5);
    Verify(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).Once();
  }

  static void FromNowToNextSunset()
  {
    int NbreDays = 8;
    When(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).AlwaysReturn();
    When(Method(ArduinoFake(), digitalWrite).Using(12, HIGH)).AlwaysReturn();
    When(Method(ArduinoFake(), digitalWrite).Using(12, LOW)).AlwaysReturn();
    WhenCallMockGetNextEvents();
    HandleTimeFromNowToEvent(Sunset + NbreDays*DayDuration + 5);
    Verify(Method(ArduinoFake(), digitalWrite).Using(12, LOW)).Exactly(NbreDays+1);
    Verify(Method(ArduinoFake(), digitalWrite).Using(12, HIGH)).Exactly(NbreDays);
    Verify(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).Exactly(NbreDays+1);
  }

  static void AtSunriseShouldWakeup()
  {
    When(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).AlwaysReturn();
    When(Method(ArduinoFake(), digitalWrite).Using(12, LOW)).AlwaysReturn();
    When(Method(ArduinoFake(), digitalWrite).Using(12, HIGH)).AlwaysReturn();
    WhenCallMockGetNextEvents();
    HandleTimeFromNowToEvent(Sunset + Night);
    Verify(Method(ArduinoFake(), digitalWrite));
  }
};


int main(int argc, char *argv[])
{
  UNITY_BEGIN();
  RUN_TEST(test_get_epochtime);

  RUN_TEST(TEST_HANDLE_TIME::AtSunsetShouldGoToSleep);
  RUN_TEST(TEST_HANDLE_TIME::AtSunsetShouldGetNewSolarEvents);
  RUN_TEST(TEST_HANDLE_TIME::AtSunsetHardwareFailureShouldGoToSleep);
  RUN_TEST(TEST_HANDLE_TIME::OverSunsetShouldCallSleepOnce);

  RUN_TEST(TEST_HANDLE_TIME::AtSunriseShouldWakeup);

  RUN_TEST(TEST_HANDLE_TIME::FromNowToNextSunset);

  return UNITY_END();
}