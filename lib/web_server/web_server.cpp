#include "web_server.h"
#include "build_page.h"
#include "OTA_update.h"

WEBSERVER::WEBSERVER()
{
  m_Server = new ESP8266WebServer(80);
  m_WebSocketServer = new WebSocketsServer(81);
}


WEBSERVER::~WEBSERVER() {
  delete m_WebSocketServer;
  m_Server->stop();
  delete m_Server;
}


void WEBSERVER::Start()
{
  m_Server->on("/", HTTP_GET, [this]() {
    m_Server->send(200, "text/html", page);
  });
  m_Server->onNotFound([this]() {
    m_Server->send(404,  "text/plain", "404: Not found");
  });
  m_Server->begin();
  m_WebSocketServer->begin();
  m_WebSocketServer->onEvent([this](uint8_t num, WStype_t type, uint8_t * message, size_t length) {
    switch (type) {
    case WStype_TEXT :
    {
      if (message[0] == '0') {
        HeaterCmd(0);
        break;
      }
      else if (message[0] == '1') {
        HeaterCmd(1);
        break;
      }
      else if (message[0] == '9') {
        UpdateFirmware();
        break;
      }
      else if (message[0] == '8') {
        ResetDivEnergy();
      }
    }
    default: break;
  }
});
  m_DivertedEnergy = 0;
}


void WEBSERVER::HandleClient()
{
  m_Server->handleClient();
  m_WebSocketServer->loop();
}


void WEBSERVER::ResetDivEnergy()
{
  m_DivertedEnergy = 0;
}


float WEBSERVER::UpdateDivEnergy(float aDivEnergy)
{
  m_DivertedEnergy += aDivEnergy;
  return m_DivertedEnergy;
}


void WEBSERVER::HeaterCmd(const int order)
{
  Serial.print(order);
}


void WEBSERVER::UpdateFirmware()
{
  OTAUpdate(this);
  for (size_t i = 0; i < 1200; i++)
  {
    HandleOTAUpdate();
    delay(100);
  }
}


void WEBSERVER::BroadcastTXT(const String aStr) {
  m_WebSocketServer->broadcastTXT(aStr.c_str(), aStr.length());
}