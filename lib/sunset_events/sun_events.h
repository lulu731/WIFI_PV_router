#ifndef _SUNEVENTS_H_
#define _SUNEVENTS_H_

#include <ctime>

class SUN_EVENT
{
public:
  void GetNextEvents(const time_t& aTime, time_t& aSunrise, time_t& aSunset);
};

#endif