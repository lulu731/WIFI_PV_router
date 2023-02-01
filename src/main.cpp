#include "secrets.h"
#include "web_server.h"
#include "wifi_manager.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266mDNS.h>

#define LOCAL_IP PVROUTER_IP
#define SERVER_NAME PVROUTER_NAME

WEBSERVER WebServer;

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

  WebServer.Start();
  #ifdef DEBUG_HARD
    Serial.println("HTTP server started");
  #endif
}

StaticJsonDocument<512> doc;
String str;

void loop()
{
  MDNS.update();
  WebServer.HandleClient();

  // read text in Serial buffer and send to client
  if(Serial.available() > 0)
  {
    DeserializationError error = deserializeJson(doc, Serial);
    if (error)
    {
      str = "Error: deserializing from Wemos : " + String(error.c_str());
    }
    else
    {
      doc["divertedEnergy"] = WebServer.UpdateDivEnergy(doc["divertedEnergy"]);
      str.clear();
      serializeJson(doc, str);
    }
    WebServer.BroadcastTXT(str);
  }
}