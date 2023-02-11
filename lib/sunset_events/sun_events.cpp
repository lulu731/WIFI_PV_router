#include "sun_events.h"
#include "position.h"
#include <sunset.h>
#include <ctime>


void SUN_EVENT::GetNextEvents(const time_t& aTime, time_t& aSunrise, time_t& aSunset)
{
  SunSet Sun;
  double latitude = LAT;
  double longitude = LONG;

  Sun.setPosition(latitude, longitude, 0);

  tm* timeinfo = gmtime(&aTime);
  timeinfo->tm_mday += 1;
  timeinfo->tm_min = 0;
  timeinfo->tm_sec = 0;
  Sun.setCurrentDate(timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday);
  int totalMinutes = Sun.calcSunrise();
  timeinfo->tm_hour = totalMinutes / 60;
  timeinfo->tm_min = totalMinutes % 60;
  aSunrise = mktime(timeinfo);

  totalMinutes = Sun.calcSunset();
  timeinfo->tm_hour = totalMinutes / 60;
  timeinfo->tm_min = totalMinutes % 60;
  aSunset = mktime(timeinfo);
}