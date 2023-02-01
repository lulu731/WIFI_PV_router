#include "OTA_update.h"
#include <ArduinoOTA.h>


void OTAUpdate(WEBSERVER* aWServer) {
  aWServer->BroadcastTXT("Updating requested !\n");
  aWServer->BroadcastTXT("IP address: " + WiFi.localIP().toString() + "\n");

  ArduinoOTA.onStart([aWServer]() {
    aWServer->BroadcastTXT("Starting OTA update\n");
  });

  ArduinoOTA.onEnd([]() {
    ESP.restart();
  });

  ArduinoOTA.onProgress([aWServer](unsigned int progress, unsigned int total) {
    aWServer->BroadcastTXT("OTA update progress: " + String(progress) + "/" + String(total) +"\n");
  });

  ArduinoOTA.onError([aWServer](ota_error_t error) {
    aWServer->BroadcastTXT("OTA update error: " + String(error) +"\n");
  });

  ArduinoOTA.begin();
}


void HandleOTAUpdate()
{
  ArduinoOTA.handle();
}