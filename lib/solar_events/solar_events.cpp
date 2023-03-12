#include "solar_events.h"
#include "position.h"
#include <sunset.h>
#include <ctime>

void SUN_EVENT::GetEvents(const time_t& aTime, time_t& aSunrise, time_t& aSunset) {
  SunSet Sun;
  double latitude = LAT;
  double longitude = LONG;

  Sun.setPosition(latitude, longitude, 0);

  tm* timeInfo = gmtime(&aTime);
  timeInfo->tm_sec = 0;

  Sun.setCurrentDate(timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday);
  int totalMinutes = Sun.calcSunrise();
  timeInfo->tm_hour = totalMinutes / 60;
  timeInfo->tm_min = totalMinutes % 60;
  aSunrise = mktime(timeInfo);

  totalMinutes = Sun.calcSunset();
  timeInfo->tm_hour = totalMinutes / 60;
  timeInfo->tm_min = totalMinutes % 60;
  aSunset = mktime(timeInfo);
}

void SUN_EVENT::GetNextEvents(const time_t& aTime, time_t& aSunrise, time_t& aSunset)
{
  const int SECONDS_PER_DAY = 24 * 60 * 60;
  GetEvents(aTime, aSunrise, aSunset);
  if (aSunrise <= aTime)
  {
    time_t anotherSunset;
    GetEvents(aTime + SECONDS_PER_DAY, aSunrise, anotherSunset);
  }

  if (aSunset <= aTime)
  {
    time_t anotherSunrise;
    GetEvents(aTime + SECONDS_PER_DAY, anotherSunrise, aSunset);
  }
}