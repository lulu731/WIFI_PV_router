#ifndef _WIFI_MGR_
#define _WIFI_MGR_

#include <IPAddress.h>

class WIFI_MGR
{
  private:
  IPAddress m_local_IP, m_gateway, m_subnet;
  const int MAX_TRY = 30; //Number times to try to connect
  public:
  WIFI_MGR( const String& aLocalIP, const String& aGateway, const String& aSubnet );
  bool Connect( const char* aSsid, const char* aPasswd );
  void AP( const char* aAPssid, const char* aAPpassword );
};

#endif