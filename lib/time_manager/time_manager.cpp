#include "time_manager.h"
#include "solar_events.h"
#include <Arduino.h>

#ifndef UNIT_TEST
  #include "ntp_time.h"
#endif

  TIME_MGR::TIME_MGR(TIME_CLIENT_ITF* a_Time_Client, SUN_EVENTS_ITF* a_Solar_Events) :
    m_Time_Client(a_Time_Client), m_Solar_Events(a_Solar_Events)
  {
  }

#ifndef UNIT_TEST
  TIME_MGR::TIME_MGR()
  {
    m_Time_Client = new TIME_CLIENT("194.158.119.97");
    m_Solar_Events = new SUN_EVENT();
  }

  TIME_MGR::~TIME_MGR()
  {
    delete m_Time_Client;
    delete m_Solar_Events;
  }
#endif


void TIME_MGR::Init(const int a_DayDuration)
{
  m_Time_Client->Init();
  m_Solar_Events->GetNextEvents(m_Time_Client->GetEpochTime(), m_Sunrise, m_Sunset);
  if (m_Time_Client->GetEpochTime() < m_Sunset - a_DayDuration)
  {
    m_IsSleeping = false;
    m_Sunset -= a_DayDuration;
  }
}


time_t TIME_MGR::GetTime()
{
  return m_Time_Client->GetEpochTime();
}


void TIME_MGR::GetNextSolarEvents()
{
  m_Solar_Events->GetNextEvents(m_Time_Client->GetEpochTime(), m_Sunrise, m_Sunset);
}


bool TIME_MGR::HandleTime()
{
  // Time crossing sunset => sleep
  if (m_Time_Client->GetEpochTime() >= m_Sunset && !m_IsSleeping)
  {
    GetNextSolarEvents();
    digitalWrite(12, HIGH);
    Serial.print('9'); // command to sleep device
    m_IsSleeping = true;
  }
  else
  // Time crossing sunrise => wake up
  {
    if (m_Time_Client->GetEpochTime() >= m_Sunrise && m_IsSleeping)
    {
      digitalWrite(12, LOW); // wake up the device
      m_IsSleeping = false;
    }
  }

  return m_IsSleeping;
}