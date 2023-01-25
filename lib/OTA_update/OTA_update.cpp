#include "OTA_update.h"
#include "web_socket.h"
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>


void OTAUpdate() {
  WebSocketBroadcastTXT("Updating requested !\n");
  WebSocketBroadcastTXT("IP address: " + WiFi.localIP().toString() + "\n");

  ArduinoOTA.onStart([]() {
    WebSocketBroadcastTXT("Starting OTA update\n");
  });

  ArduinoOTA.onEnd([]() {
    ESP.restart();
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    WebSocketBroadcastTXT("OTA update progress: " + String(progress) + "/" + String(total) +"\n");
  });

  ArduinoOTA.onError([](ota_error_t error) {
    WebSocketBroadcastTXT("OTA update error: " + String(error) +"\n");
  });

  ArduinoOTA.begin();
}


void HandleOTAUpdate()
{
  ArduinoOTA.handle();
}