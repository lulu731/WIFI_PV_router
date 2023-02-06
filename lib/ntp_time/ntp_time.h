#ifndef __NTP_TIME_H__
#define __NTP_TIME_H__

#include <NTPClient.h>
#include <WiFiUdp.h>

class TIME_CLIENT {
public:
  TIME_CLIENT(const String&);
  ~TIME_CLIENT();
  bool Init();
  unsigned long GetEpochTime();
private:
  WiFiUDP m_WifiUdp;
  NTPClient* m_NtpClient;
};

#endif