#ifndef _SOLAR_EVENTS_ITF_H_
#define _SOLAR_EVENTS_ITF_H_

#include <ctime>

class SUN_EVENTS_ITF
{
public:
  virtual ~SUN_EVENTS_ITF(){};
  virtual void GetNextEvents(const time_t& aTime, time_t& aSunrise, time_t& aSunset) = 0;
};

#endif