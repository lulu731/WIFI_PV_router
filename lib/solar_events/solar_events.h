#ifndef _SOLAR_EVENTS_H_
#define _SOLAR_EVENTS_H_

#include "solar_events_itf.h"
#include <ctime>

class SUN_EVENT : public SUN_EVENTS_ITF
{
public:
  void GetNextEvents(const time_t& aTime, time_t& aSunrise, time_t& aSunset);
};

#endif