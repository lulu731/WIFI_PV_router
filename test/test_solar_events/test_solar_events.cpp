#include "solar_events.h"
#include <unity.h>
#include <time.h>

tm        TimeInfo;
time_t    sunrise, sunset;
SUN_EVENT SunEvent;

void setUp( void )
{
  TimeInfo.tm_year = 2023 - 1900;
  TimeInfo.tm_mon = 2 - 1;
  TimeInfo.tm_mday = 10;
  // TimeInfo.tm_hour defined in each test
  TimeInfo.tm_min = 0;
  TimeInfo.tm_sec = 0;
  TimeInfo.tm_isdst = 0;
  sunrise = 0;
  sunset = 0;
}

void GetNextSunriseSunsetFromNoon()
{
  TimeInfo.tm_hour = 12;
  time_t now = mktime( &TimeInfo );

  SunEvent.GetNextEvents( now, sunrise, sunset );
  //Test next solar events verify : now < sunset < sunrise
  TEST_ASSERT_GREATER_THAN_UINT32( now, sunset );
  TEST_ASSERT_GREATER_THAN_UINT32( sunset, sunrise );
  TEST_ASSERT_UINT32_WITHIN( 12 * 3600, now, sunset );
  TEST_ASSERT_UINT32_WITHIN( 24 * 3600, now, sunrise );
}

void GetNextSunriseSunsetFromOneOClock()
{
  TimeInfo.tm_hour = 1;
  time_t now = mktime( &TimeInfo );

  SunEvent.GetNextEvents( now, sunrise, sunset );
  //Test next solar events verify : now < sunrise < sunset
  TEST_ASSERT_GREATER_THAN_UINT32( now, sunrise );
  TEST_ASSERT_GREATER_THAN_UINT32( sunrise, sunset );
  TEST_ASSERT_UINT32_WITHIN( 12 * 3600, now, sunrise );
  TEST_ASSERT_UINT32_WITHIN( 12 * 3600, sunrise, sunset );
}

void GetNextSunriseSunsetFrom22OClock()
{
  TimeInfo.tm_hour = 22;
  time_t now = mktime( &TimeInfo );

  SunEvent.GetNextEvents( now, sunrise, sunset );
  //Test next solar events verify : now < sunrise < sunset
  TEST_ASSERT_GREATER_THAN_UINT32( now, sunrise );
  TEST_ASSERT_GREATER_THAN_UINT32( sunrise, sunset );
  TEST_ASSERT_UINT32_WITHIN( 12 * 3600, now, sunrise );
  TEST_ASSERT_UINT32_WITHIN( 12 * 3600, sunrise, sunset );
}

void GetNextSunriseSunsetFromSunset()
{
  TimeInfo.tm_hour = 12;
  time_t now = mktime( &TimeInfo );

  SunEvent.GetNextEvents( now, sunrise, sunset );
  now = sunset;
  SunEvent.GetNextEvents( now, sunrise, sunset );
  //Test next solar events verify : now < sunrise < sunset
  TEST_ASSERT_GREATER_THAN_UINT64( now, sunrise );
  TEST_ASSERT_GREATER_THAN_UINT64( now, sunset );
  TEST_ASSERT_GREATER_THAN_UINT64( sunrise, sunset );
  TEST_ASSERT_UINT32_WITHIN( 16 * 3600, now, sunrise );
  TEST_ASSERT_UINT32_WITHIN( 24 * 3600 + 600, now, sunset );
}

void GetNextSunriseSunsetFromSunsetFor3Days()
{
  TimeInfo.tm_hour = 12;
  time_t now = mktime( &TimeInfo );

  SunEvent.GetNextEvents( now, sunrise, sunset );
  for( size_t day = 0; day < 3; day++ )
  {
    now = sunset;
    SunEvent.GetNextEvents( now, sunrise, sunset );
  }

  //Test next solar events verify : now < sunrise < sunset
  TEST_ASSERT_GREATER_THAN_UINT32( now, sunrise );
  TEST_ASSERT_GREATER_THAN_UINT32( now, sunset );
  TEST_ASSERT_GREATER_THAN_UINT32( sunrise, sunset );
  TEST_ASSERT_UINT32_WITHIN( 16 * 3600, now, sunrise );
  TEST_ASSERT_UINT32_WITHIN( 24 * 3600 + 600, now, sunset );
}

int main( int argc, char* argv[] )
{
  UNITY_BEGIN();
  RUN_TEST( GetNextSunriseSunsetFromNoon );
  RUN_TEST( GetNextSunriseSunsetFromOneOClock );
  RUN_TEST( GetNextSunriseSunsetFrom22OClock );
  RUN_TEST( GetNextSunriseSunsetFromSunset );
  RUN_TEST( GetNextSunriseSunsetFromSunsetFor3Days );
  UNITY_END();
}