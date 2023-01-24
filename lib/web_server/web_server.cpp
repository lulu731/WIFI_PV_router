#include "web_server.h"
#include "build_page.h"
#include "OTA_update.h"
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);


void HandleRoot()
{
  server.send(200, "text/html", page);
}


void HandleHeaterCmd()
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


void HandleUpdate()
{
  if (server.hasArg("ota_update") && server.arg("ota_update")=="Update")
  {
    OTAUpdate(server);
    for (size_t i = 0; i < 1200; i++)
    {
      HandleOTAUpdate();
      delay(100);
    }
  }
}


void HandleNotFound()
{
  server.send(404, "text/plain", "404: Not found");
}


void StartWebserver()
{
  server.on("/", HandleRoot);
  server.on("/heater", HTTP_POST, HandleHeaterCmd);
  server.on("/ota_update", HandleUpdate);
  server.onNotFound(HandleNotFound);
  server.begin();
}


void ServerHandleClient()
{
  server.handleClient();
}