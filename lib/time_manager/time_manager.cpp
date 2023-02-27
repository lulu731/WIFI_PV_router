#include "time_manager.h"
#include "solar_events.h"
#include <Arduino.h>

#ifndef UNIT_TEST
  #include "ntp_time.h"
#endif

  TIME_MGR::TIME_MGR(TIME_CLIENT_ITF* a_Time_Client, SUN_EVENTS_ITF* a_Solar_Events, const int a_DayDuration) :
    m_Time_Client(a_Time_Client), m_Solar_Events(a_Solar_Events)
  {
    m_Time_Client->Init();
    m_Solar_Events->GetNextEvents(m_Time_Client->GetEpochTime(), m_Sunrise, m_Sunset);
    if (m_Time_Client->GetEpochTime() < m_Sunset - a_DayDuration)
    {
      m_IsSleeping = true;
      m_Sunset -= a_DayDuration;
    }
    else
      m_IsSleeping = false;
  }

#ifndef UNIT_TEST
  TIME_MGR::TIME_MGR()
  {
    TIME_MGR(new TIME_CLIENT("194.158.119.97"), new SUN_EVENT);
  }

  TIME_MGR::~TIME_MGR()
  {
    delete m_Time_Client;
    delete m_Solar_Events;
  }
#endif


time_t TIME_MGR::GetTime()
{
  return m_Time_Client->GetEpochTime();
}


void TIME_MGR::GetNextSolarEvents()
{
  m_Solar_Events->GetNextEvents(m_Time_Client->GetEpochTime(), m_Sunrise, m_Sunset);
}


void TIME_MGR::HandleTime()
{
  if (m_Time_Client->GetEpochTime() >= m_Sunset && !m_IsSleeping) {
    GetNextSolarEvents();
    digitalWrite(12, HIGH);
    Serial.print('9');
    m_IsSleeping = true;
  }
  else
  {
    if (m_Time_Client->GetEpochTime() >= m_Sunrise && m_IsSleeping) {
      digitalWrite(12, LOW);
      m_IsSleeping = false;
    }
  }
}