#include "secrets.h"
#include "web_server.h"
#include "wifi_manager.h"
#include "time_manager.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266mDNS.h>
#include <LittleFS.h>

#define LOCAL_IP PVROUTER_IP
#define SERVER_NAME PVROUTER_NAME

WEBSERVER WebServer;
TIME_MGR TimeMgr;

String LastJson = "{\"realPower1\": 0.0, \"realPower2\": 0.0, \"divertedEnergy\": 0.0}";

void setup()
{
  pinMode(12, OUTPUT);
  digitalWrite(12, LOW);

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

  LittleFS.begin();

  WebServer.Start(&LastJson);
  #ifdef DEBUG_HARD
    Serial.println("HTTP server started");
  #endif

  delay(60000); // wait for Mega initializing

  TimeMgr.Init();
  #ifdef DEBUG_HARD
    Serial.print("Time client started: ");
    time_t t = TimeMgr.GetTime();
    Serial.println(String(std::ctime(&t)));
    time_t sunrise = TimeMgr.GetSunrise();
    Serial.print("Sunrise: ");
    Serial.println(String(std::ctime(&sunrise)));
    Serial.print("Sunset: ");
    time_t sunset = TimeMgr.GetSunset();
    Serial.println(String(std::ctime(&sunset)));
  #endif
}

StaticJsonDocument<512> doc;
String str;
bool WasSleeping = false;

void loop()
{
  MDNS.update();
  WebServer.HandleClient();
  bool IsSleeping = TimeMgr.HandleTime();

  if (WasSleeping != IsSleeping)
  {
    WasSleeping = IsSleeping;
    const time_t t = TimeMgr.GetTime();
    String str = String(std::ctime(&t));
    if (!IsSleeping)
    {
      WebServer.BroadcastTXT("Waking up : " + str);
      WebServer.ResetDivEnergy();
      #ifdef DEBUG_HARD
        time_t sunrise = TimeMgr.GetSunrise();
        Serial.print("Sunrise: ");
        Serial.println(String(std::ctime(&sunrise)));
        Serial.print("Sunset: ");
        time_t sunset = TimeMgr.GetSunset();
        Serial.println(String(std::ctime(&sunset)));
      #endif
    }
    else
    {
      WebServer.BroadcastTXT("Sleeping : " + str);
      #ifdef DEBUG_HARD
        time_t sunrise = TimeMgr.GetSunrise();
        Serial.print("Sunrise: ");
        Serial.println(String(std::ctime(&sunrise)));
        Serial.print("Sunset: ");
        time_t sunset = TimeMgr.GetSunset();
        Serial.println(String(std::ctime(&sunset)));
      #endif
    }
    deserializeJson(doc, LastJson);
    doc["isSleeping"] = IsSleeping;
    LastJson.clear();
    serializeJson(doc, LastJson);
    WebServer.BroadcastTXT(LastJson);
  }

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
      doc["isSleeping"] = IsSleeping;
      str.clear();
      serializeJson(doc, str);
      LastJson = str;
    }
    WebServer.BroadcastTXT(str);
  }
}