#include "sun_events.h"
#include <unity.h>
#include <time.h>

void setUp(void)
{
  //SUN_EVENT SunEvent;
}


void tearDown(void)
{
  // Nothing to do.
}

void test_get_next_sunrise_sunset()
{
  tm timeinfo;
  timeinfo.tm_year = 2023 - 1900;
  timeinfo.tm_mon = 2 - 1;
  timeinfo.tm_mday = 10;
  timeinfo.tm_hour = 17;
  timeinfo.tm_min = 0;
  timeinfo.tm_sec = 0;
  timeinfo.tm_isdst = 0;

  time_t now = mktime(&timeinfo);
  time_t sunrise, sunset;
  SUN_EVENT SunEvent;
  SunEvent.GetNextEvents(now, sunrise, sunset);
  //Test next solar events verify : now < sunrise < sunset
  TEST_ASSERT_GREATER_THAN_UINT32(now, sunrise);
  TEST_ASSERT_GREATER_THAN_UINT32(sunrise, sunset);
  TEST_ASSERT_UINT32_WITHIN(24*3600, now, sunrise);
  TEST_ASSERT_UINT32_WITHIN(24*3600, sunrise, sunset);
}

int main(int argc, char *argv[])
{
  UNITY_BEGIN();
  RUN_TEST(test_get_next_sunrise_sunset);
  UNITY_END();
}