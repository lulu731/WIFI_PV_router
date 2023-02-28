#ifndef _TIME_MGR_H_
#define _TIME_MGR_H_

#include "time_mgr_itf.h"
#include "ntp_time_itf.h"
#include "solar_events_itf.h"
#include <ctime>

class TIME_MGR : public TIME_MGR_ITF
{
friend class TEST_TIME_MGR;

private:
  TIME_CLIENT_ITF* m_Time_Client;
  SUN_EVENTS_ITF* m_Solar_Events;
  time_t m_Sunrise, m_Sunset;
  bool m_IsSleeping;
  void GetNextSolarEvents();

  //void ReadyWakeUp();
protected:
  TIME_MGR(TIME_CLIENT_ITF* a_Time_Client, SUN_EVENTS_ITF* a_Solar_Events, const int a_DayDuration = 24 * 60 * 60);

public:
#ifndef UNIT_TEST
  TIME_MGR();
  ~TIME_MGR();
#endif
  time_t GetTime();
  bool HandleTime();

  //void Sleep();
};

#endif