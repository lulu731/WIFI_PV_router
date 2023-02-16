#ifndef _SOLAR_EVENTS_H_
#define _SOLAR_EVENTS_H_

#include <ctime>

class SUN_EVENT
{
public:
  static void GetNextEvents(const time_t& aTime, time_t& aSunrise, time_t& aSunset);
};

#endif