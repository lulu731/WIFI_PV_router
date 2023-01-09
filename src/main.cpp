#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "secrets.h"
#include "OTA_update.h"
#include "web_server.h"

//WebServer creation
ESP8266WebServer server(80);

// function prototypes for HTTP handlers
void handleRoot();
void handleHeaterCmd();
void handleOTAUpdate();
void handleNotFound();

void setup()
{
  Serial.begin(115200);
  delay(10);

  // Start the access point
  WiFi.softAP(ap_ssid, ap_password);
  #ifdef DEBUG_HARD
    Serial.print("AccessPt_IP address:\t");
    Serial.println(WiFi.softAPIP());
  #endif

  // Connect to WiFi network
  WiFi.enableSTA(true);
  WiFi.begin(ssid, password);             // Connect to the network
  #ifdef DEBUG_HARD
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.println(" ...");
  #endif

  int8_t connectionStatus = WiFi.waitForConnectResult();
  #ifdef DEBUG_HARD
    if (connectionStatus != WL_CONNECTED) {
      Serial.print("Connection Failed! ");
      Serial.println(connectionStatus);
    }
    else {
      Serial.println('\n');
      Serial.println("Connection established!");
      Serial.print("IP address:\t");
      Serial.println(WiFi.localIP());
    }
  #endif

  bool isMDNSStarted = MDNS.begin("pvrouter");
  #ifdef DEBUG_HARD
    Serial.println("mDNS responder started ?");
    Serial.println(isMDNSStarted);
  #endif

  // Start WebServer
  server.on("/", handleRoot);
  server.on("/heater", HTTP_POST, handleHeaterCmd);
  server.on("/ota_update", handleOTAUpdate);
  server.onNotFound(handleNotFound);
  server.begin();
  #ifdef DEBUG_HARD
    Serial.println("HTTP server started");
  #endif
}

void loop()
{
  MDNS.update();
  server.handleClient();

  #ifdef WIFI_UNUSED
    OtaUpdate();
    // 2 min to update sketch
    for (size_t i = 0; i < 1200; i++)
    {
      HandleOtaUpdate();
      delay(100);
    }
    // no update go to sleep
    ESP.deepSleep(0);
  #endif
}

void handleRoot()
{
  server.send(200, "text/html",
  "<form action=\"/heater\" method=\"POST\">\
    <input type=\"submit\" name=\"heater\" value=\"ON\">\
    <input type=\"submit\" name=\"heater\" value=\"OFF\">\
  </form>\
  <br></br>\
  <form action=\"/ota_update\" method=\"GET\">\
    <input type=\"submit\" name=\"ota_update\" value=\"Update\">\
  ");
}

void handleHeaterCmd()
{
  if (server.hasArg("heater"))
  {
    int order = 0;
    if (server.arg("heater")=="ON") order = 1;
    Serial.print(order);
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleOTAUpdate()
{
  if (server.hasArg("ota_update") && server.arg("ota_update")=="Update")
  {
    OtaUpdate();
    for (size_t i = 0; i < 1200; i++)
    {
      HandleOtaUpdate();
      delay(100);
    }
  }
}

void handleNotFound()
{
  server.send(404, "text/plain", "404: Not found");
}