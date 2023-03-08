#include "time_manager.h"
#include "ntp_time_itf.h"
#include "solar_events_itf.h"
#include <ArduinoFake.h>
#include <unity.h>

using namespace fakeit;

const int Daylight = 10, DayDuration = 40, Night = 30;
time_t Now, Sunrise, Sunset;

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
    else // GetNextEvents is called at sunset
    {
      aSunrise = aTime + Night;
      aSunset = aTime + DayDuration;
      Args[0] = aTime;
      Args[1] = aSunrise;
      Args[2] = aSunset;
    }
  });
}

void InitMockCalls()
{
  WhenCallMockGetNextEvents();
  When(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).AlwaysReturn();
  When(Method(ArduinoFake(), digitalWrite).Using(12, HIGH)).AlwaysReturn();
  When(Method(ArduinoFake(), digitalWrite).Using(12, LOW)).AlwaysReturn();
}

class TEST_TIME_MGR
{
private:
  TIME_MGR* m_Time_Mgr = new TIME_MGR(tc, se);

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

  void Init()
  {
    m_Time_Mgr->Init(DayDuration);
  }

  void TestGetEpochtime()
  {
    Method(Time_Client_Mock, GetEpochTime) = Now;
    auto aEpochTime = m_Time_Mgr->GetTime();
    TEST_ASSERT_EQUAL(Now, aEpochTime);
  }

  void AtSunsetShouldGoToSleep()
  {
    Init();
    HandleTimeFromNowToEvent(Sunset);
    Verify(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).Twice();
  }

  void AtSunsetShouldGetNewSolarEvents()
  {
    Init();
    HandleTimeFromNowToEvent(Sunset);
    std::array<time_t, 3> ExpectedArgs = {Sunset, Sunset + Night, Sunset + DayDuration};
    TEST_ASSERT_EQUAL_INT32_ARRAY(ExpectedArgs.data(), Args.data(), 3);
    Verify(Method(Solar_Events_Mock, GetNextEvents) +
      OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).Twice();
  }

  // Failure case: Sunset time is reached, but hardware failure is not able to call sleep command
  void AtSunsetHardwareFailureShouldGoToSleep()
  {
    Init();
    HandleTimeFromNowWithFailureAtEvent(Sunset + 2 - Now, Sunset);
    Verify(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).Twice(); // sleep command is called at init() and sunset
  }

  void OverSunsetShouldCallSleepTwice()
  {
    Init();
    HandleTimeFromNowToEvent(Sunset + 5);
    Verify(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).Twice();
  }

  void AtSunriseShouldWakeup()
  {
    Init();
    HandleTimeFromNowToEvent(Sunset + Night);
    Verify(Method(ArduinoFake(), digitalWrite));
  }

  void FromNowTo8thSunset()
  {
    int NbreDays = 8;
    Init();
    HandleTimeFromNowToEvent(Sunset + NbreDays*DayDuration + 5);
    Verify(Method(ArduinoFake(), digitalWrite).Using(12, HIGH)).Exactly(NbreDays+2);
    Verify(Method(ArduinoFake(), digitalWrite).Using(12, LOW)).Exactly(NbreDays+1);
    Verify(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).Exactly(NbreDays+2);
  }

  void FromNoonToSunset()
  {
    Now = Sunrise + 5; // Now at noon
    Sunrise = Sunrise + DayDuration;
    When(Method(Time_Client_Mock, GetEpochTime)).AlwaysReturn(Now);
    Init();
    HandleTimeFromNowToEvent(Sunset);
    Verify(Method(ArduinoFake(), digitalWrite).Using(12, HIGH)).Once();
    Verify(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).Once();
    Verify(Method(ArduinoFake(), digitalWrite).Using(12, LOW)).Once();
  }

  void FromNoonTo8thSunset()
  {
    int NbreDays = 8;
    Now = Sunrise + 5; // Now at noon
    When(Method(Time_Client_Mock, GetEpochTime)).AlwaysReturn(Now);
    Sunrise = Sunrise + DayDuration;
    Init();
    HandleTimeFromNowToEvent(Sunset + NbreDays*DayDuration + 5);
    Verify(Method(ArduinoFake(), digitalWrite).Using(12, HIGH)).Exactly(NbreDays+1);
    Verify(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).Exactly(NbreDays+1);
    Verify(Method(ArduinoFake(), digitalWrite).Using(12, LOW)).Exactly(NbreDays+1);
  }

  void InitAtNightShouldGoToSleep()
  {
    Init();
    Verify(Method(ArduinoFake(), digitalWrite).Using(12, HIGH)).Once();
    Verify(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)).Using('9')).Once();
  }
};

TEST_TIME_MGR* TestTimeMgr;

void setUp(void)
{
  Now = 1676306813;
  Sunrise = 1676306820;
  Sunset = Sunrise + Daylight;
  ArduinoFakeReset();
  Solar_Events_Mock.ClearInvocationHistory();
  Time_Client_Mock.ClearInvocationHistory();

  tc = &Time_Client_Mock.get();
  se = &Solar_Events_Mock.get();

  Method(Time_Client_Mock, Init) = true;
  When(Method(Time_Client_Mock, GetEpochTime)).AlwaysReturn(Now);

  TestTimeMgr = new TEST_TIME_MGR();
  InitMockCalls();
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
  RUN_TEST([](){TestTimeMgr->OverSunsetShouldCallSleepTwice();});
  RUN_TEST([](){TestTimeMgr->AtSunriseShouldWakeup();});
  RUN_TEST([](){TestTimeMgr->FromNowTo8thSunset();});
  RUN_TEST([](){TestTimeMgr->FromNoonToSunset();});
  RUN_TEST([](){TestTimeMgr->FromNoonTo8thSunset();});
  RUN_TEST([](){TestTimeMgr->InitAtNightShouldGoToSleep();});
  return UNITY_END();
}