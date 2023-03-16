#ifndef __NTP_TIME_H__
#define __NTP_TIME_H__

#include "ntp_time_itf.h"
#include <NTPClient.h>
#include <WiFiUdp.h>

class TIME_CLIENT : public TIME_CLIENT_ITF
{
  public:
  TIME_CLIENT( const String& );
  virtual ~TIME_CLIENT();
  bool          Init();
  unsigned long GetEpochTime();

  private:
  WiFiUDP    m_WifiUdp;
  NTPClient* m_NtpClient;
};

#endif