#include <ESP8266WiFi.h>
#include "wifi_manager.h"


WIFI_MGR::WIFI_MGR(const String& aLocalIP, const String& aGateway, const String& aSubnet)
{
    m_local_IP.fromString(aLocalIP);
    m_gateway.fromString(aGateway);
    m_subnet.fromString(aSubnet);
}


bool WIFI_MGR::Connect(const char* aSsid, const char* aPasswd)
{
  WiFi.config(m_local_IP, m_gateway, m_subnet);
  WiFi.begin(aSsid, aPasswd);
  #ifdef DEBUG_HARD
    Serial.print("Connecting to ");
    Serial.print(aSsid);
    Serial.println(" ...");
  #endif

  int tryingNber(0);
  while (WiFi.waitForConnectResult()!=WL_CONNECTED)
  {
    delay(1000);
    tryingNber++;
    if (tryingNber == MAX_TRY) break;
  }

  #ifdef DEBUG_HARD
    if (tryingNber == MAX_TRY)
    {
      Serial.println("Connection established!");
      Serial.print("IP address:\t");
      Serial.println(WiFi.localIP());
    }
    else
      Serial.print("Connection failed!");
  #endif

  return !(tryingNber == MAX_TRY);
}


void WIFI_MGR::AP(const char* aAPssid, const char* aAPpassword)
{
  WiFi.softAP(aAPssid, aAPpassword);
  #ifdef DEBUG_HARD
    Serial.print("AccessPt_IP address : \t");
    Serial.println(WiFi.softAPIP());
  #endif
}