#include "ntp_time.h"

TIME_CLIENT::TIME_CLIENT( const String& a_IPAdress )
{
  IPAddress ipAdress;
  ipAdress.fromString( a_IPAdress );
  m_NtpClient = new NTPClient( m_WifiUdp, ipAdress, 0, 1000 );
}


TIME_CLIENT::~TIME_CLIENT()
{
  delete m_NtpClient;
}


bool TIME_CLIENT::Init()
{
  m_NtpClient->begin();
  bool isInit = false;
  for( int i = 0; i < 10; i++ )
  {
    delay( 1000 );
    isInit = m_NtpClient->update();
    if( isInit )
    {
      m_NtpClient->setUpdateInterval( 24 * 3600 * 1000 ); //set update interval to one day
      break;
    }
  }
  return isInit;
}


unsigned long TIME_CLIENT::GetEpochTime()
{
  m_NtpClient->update();
  return m_NtpClient->getEpochTime();
}