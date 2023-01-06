#include <ArduinoOTA.h>
#include "OTA_update.h"

void OtaUpdate() {
  server.setContentLength(10000);
  server.send(200, "text/html", "Updating requested !");
  server.sendContent("Ready");
  server.sendContent("IP address: ");
  server.sendContent(WiFi.localIP().toString());
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

void HandleOtaUpdate()
{
  ArduinoOTA.handle();
}
