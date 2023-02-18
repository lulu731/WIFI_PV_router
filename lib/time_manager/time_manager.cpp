#include "time_manager.h"
#include "solar_events.h"

#ifdef UNIT_TEST
  #include <ArduinoFake.h>
#else
  #include <Arduino.h>
#endif

#ifdef UNIT_TEST
  TIME_MGR::TIME_MGR(TIME_CLIENT_ITF& a_Time_Client, SUN_EVENTS_ITF& a_Solar_Events) :
    m_Time_Client(a_Time_Client), m_Solar_Events(a_Solar_Events)
  {
    m_Time_Client.Init();
    m_Solar_Events.GetNextEvents(m_Time_Client.GetEpochTime(), m_Sunrise, m_Sunset);
  }
#else
  // TODO : add constructor
  /*TIME_MGR::TIME_MGR() :
    m_Time_Client(), m_Solar_Events()
  {
    m_Time_Client.Init();
  }*/
#endif

TIME_MGR::~TIME_MGR()
{
}


time_t TIME_MGR::GetTime()
{
  return m_Time_Client.GetEpochTime();
}


void TIME_MGR::GetNextSolarEvents()
{
  m_Solar_Events.GetNextEvents(m_Time_Client.GetEpochTime(), m_Sunrise, m_Sunset);
}


void TIME_MGR::HandleTime()
{
  if (m_Time_Client.GetEpochTime() >= m_Sunset) {
    Serial.print('9');
  }
}