#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include "OTA_update.h"


void OTAUpdate() {
  ArduinoOTA.onStart([]() {

  });

  ArduinoOTA.onEnd([]() {
    ESP.restart();
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {

  });

  ArduinoOTA.onError([](ota_error_t error) {

  });

  ArduinoOTA.begin();
}


void OTAUpdate(ESP8266WebServer& aServer) {
  aServer.setContentLength(10000);
  aServer.send(200, "text/html", "Updating requested !");
  aServer.sendContent("Ready");
  aServer.sendContent("IP address: ");
  aServer.sendContent(WiFi.localIP().toString());
  OTAUpdate();
}


void HandleOTAUpdate()
{
  ArduinoOTA.handle();
}