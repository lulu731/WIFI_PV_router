#include "OTA_update.h"
#include "secrets.h"
#include "web_server.h"
#include "web_socket.h"
#include "wifi_manager.h"
#include <Arduino.h>
#include <ESP8266mDNS.h>

#define LOCAL_IP PVROUTER_IP
#define SERVER_NAME PVROUTER_NAME


void setup()
{
  Serial.begin(115200);
  delay(10);

  //Connect to Wifi or open Access Point
  WIFI_MGR WifiMgr(LOCAL_IP, "192.168.1.254", "255.255.255.0");
  if (!WifiMgr.Connect(ssid, password)) // Connect to the network
    WifiMgr.AP(ap_ssid, ap_password); // Start the access point if no Wifi connection

  MDNS.begin(SERVER_NAME);
  #ifdef DEBUG_HARD
    Serial.print("mDNS responder started: ");
    Serial.println(SERVER_NAME);
  #endif

  StartWebserver();
  #ifdef DEBUG_HARD
    Serial.println("HTTP server started");
  #endif

  StartWebSocket();
}

String str;
void loop()
{
  MDNS.update();
  ServerHandleClient();
  WebSocketLoop();

  // read text in Serial buffer and send to client
  if(Serial.available() > 0)
  {
    str = Serial.readString();
    WebSocketBroadcastTXT(str);
  }

  #ifdef WIFI_UNUSED
    OTAUpdate();
    // 2 min to update sketch
    for (size_t i = 0; i < 1200; i++)
    {
      HandleOTAUpdate();
      delay(100);
    }
    // no update go to sleep
    ESP.deepSleep(0);
  #endif
}