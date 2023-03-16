#ifndef _TIME_MGR_ITF_H_
#define _TIME_MGR_ITF_H_

#include "ntp_time_itf.h"
#include <ctime>

class TIME_MGR_ITF
{
  public:
  //TIME_MGR(TIME_CLIENT_ITF& a_Time_Client);
  virtual time_t GetTime() = 0;
  virtual bool   HandleTime() = 0;
  //virtual int GetNextSolarEvents() = 0;
  //void Sleep();
};

#endif