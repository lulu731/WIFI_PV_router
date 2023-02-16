#include "time_manager.h"
#include "solar_events.h"

TIME_MGR::TIME_MGR(TIME_CLIENT_ITF& a_Time_Client) : m_Time_Client(&a_Time_Client)
{
  m_Time_Client->Init();
}


TIME_MGR::~TIME_MGR()
{
}


time_t TIME_MGR::GetTime()
{
  return m_Time_Client->GetEpochTime();
}


void TIME_MGR::GetNextSolarEvents()
{
  SUN_EVENT::GetNextEvents(m_Time_Client->GetEpochTime(), m_Sunrise, m_Sunset);
}