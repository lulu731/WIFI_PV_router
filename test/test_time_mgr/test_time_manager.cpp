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

void InitMockCalls(const bool aCallDigitalWriteLow = false)
{
  When(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).AlwaysReturn();
  When(Method(ArduinoFake(), digitalWrite).Using(12, HIGH)).AlwaysReturn();
  WhenCallMockGetNextEvents();
  if (aCallDigitalWriteLow)
    When(Method(ArduinoFake(), digitalWrite).Using(12, LOW)).AlwaysReturn();
}

class TEST_TIME_MGR
{
private:
  TIME_MGR* m_Time_Mgr = new TIME_MGR(tc, se, DayDuration);

  void HandleTimeFromNowToEvent(const time_t aEvent)
  {
    for (size_t Seconds = 1; Seconds < aEvent - Now + 1; Seconds++)
    {
      When(Method(Time_Client_Mock, GetEpochTime)).AlwaysReturn(Now + Seconds);
      m_Time_Mgr->HandleTime();
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
        m_Time_Mgr->HandleTime();
    };
  }

public:
  ~TEST_TIME_MGR()
  {
    delete m_Time_Mgr;
  }

  void TestGetEpochtime()
  {
    Method(Time_Client_Mock, GetEpochTime) = Now;
    auto aEpochTime = m_Time_Mgr->GetTime();
    TEST_ASSERT_EQUAL(Now, aEpochTime);
  }

  void AtSunsetShouldGoToSleep()
  {
    // Sunset and Sunrise are initiated in Time_Manager constructor
    InitMockCalls();
    HandleTimeFromNowToEvent(Sunset);
    Verify(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).Once();
  }

  void AtSunsetShouldGetNewSolarEvents()
  {
    InitMockCalls();
    HandleTimeFromNowToEvent(Sunset);
    std::array<time_t, 3> ExpectedArgs = {Sunset, Sunset + Night, Sunset + DayDuration};
    TEST_ASSERT_EQUAL_INT32_ARRAY(ExpectedArgs.data(), Args.data(), 3);
    Verify(Method(Solar_Events_Mock, GetNextEvents) +
      OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).Once();
  }

  // Failure case: Sunset time is reached, but hardware failure is not able to call sleep command
  void AtSunsetHardwareFailureShouldGoToSleep()
  {
    InitMockCalls();
    HandleTimeFromNowWithFailureAtEvent(Sunset + 2 - Now, Sunset);
    Verify(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).Once();
  }

  void OverSunsetShouldCallSleepOnce()
  {
    InitMockCalls();
    HandleTimeFromNowToEvent(Sunset + 5);
    Verify(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).Once();
  }

  void FromNowToNextSunset()
  {
    int NbreDays = 8;
    InitMockCalls(true);
    HandleTimeFromNowToEvent(Sunset + NbreDays*DayDuration + 5);
    Verify(Method(ArduinoFake(), digitalWrite).Using(12, HIGH)).Exactly(NbreDays+1);
    Verify(Method(ArduinoFake(), digitalWrite).Using(12, LOW)).Exactly(NbreDays);
    Verify(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).Exactly(NbreDays+1);
  }

  void AtSunriseShouldWakeup()
  {
    InitMockCalls(true);
    HandleTimeFromNowToEvent(Sunset + Night);
    Verify(Method(ArduinoFake(), digitalWrite));
  }

  void TestInitDaylightSleepingParam()
  {
    Now = Sunset - DayDuration - 5;
    TEST_TIME_MGR::FromNowToNextSunset();
  }
};

TEST_TIME_MGR* TestTimeMgr;

void setUp(void)
{
  ArduinoFakeReset();
  Method(Time_Client_Mock, Init) = true;
  When(Method(Time_Client_Mock, GetEpochTime)).AlwaysReturn(Now);
  tc = &Time_Client_Mock.get();
  se = &Solar_Events_Mock.get();

  WhenCallMockGetNextEvents();
  TestTimeMgr = new TEST_TIME_MGR();
  Solar_Events_Mock.ClearInvocationHistory();
  Time_Client_Mock.ClearInvocationHistory();
}

void tearDown(void)
{
  delete TestTimeMgr;
}


int main(int argc, char *argv[])
{
  UNITY_BEGIN();
  RUN_TEST([](){TestTimeMgr->TestGetEpochtime();});
  RUN_TEST([](){TestTimeMgr->AtSunsetShouldGoToSleep();});
  RUN_TEST([](){TestTimeMgr->AtSunsetShouldGetNewSolarEvents();});
  RUN_TEST([](){TestTimeMgr->AtSunsetHardwareFailureShouldGoToSleep();});
  RUN_TEST([](){TestTimeMgr->OverSunsetShouldCallSleepOnce();});
  RUN_TEST([](){TestTimeMgr->AtSunriseShouldWakeup();});
  RUN_TEST([](){TestTimeMgr->FromNowToNextSunset();});
  RUN_TEST([](){TestTimeMgr->TestInitDaylightSleepingParam();});

  return UNITY_END();
}