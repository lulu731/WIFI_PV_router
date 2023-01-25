#include "web_server.h"
#include "build_page.h"
#include "OTA_update.h"
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);

extern float divertedEnergy;

void HandleRoot()
{
  server.send(200, "text/html", page);
}


void HandleHeaterCmd(const int order)
{
  Serial.print(order);
}


void HandleResetDivEnerg()
{
  divertedEnergy = 0;
}


void HandleUpdate()
{
  OTAUpdate();
  for (size_t i = 0; i < 1200; i++)
  {
    HandleOTAUpdate();
    delay(100);
  }
}


void HandleNotFound()
{
  server.send(404, "text/plain", "404: Not found");
}


void StartWebserver()
{
  server.on("/", HandleRoot);
  //server.on("/heater", HTTP_POST, HandleHeaterCmd);
  //server.on("/ota_update", HandleUpdate);
  server.onNotFound(HandleNotFound);
  server.begin();
}


void ServerHandleClient()
{
  server.handleClient();
}