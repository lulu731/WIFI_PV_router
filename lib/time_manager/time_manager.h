#ifndef _TIME_MGR_H_
#define _TIME_MGR_H_

#include "ntp_time_itf.h"
#include <ctime>

class TIME_MGR
{
private:
  TIME_CLIENT_ITF* m_Time_Client;
public:
  TIME_MGR(TIME_CLIENT_ITF& a_Time_Client);
  ~TIME_MGR();
  //void SetTime();
  time_t GetTime();
  void GetNextSolarEvents();
  //void SetWakeUp();
  //void Sleep();
};

#endif