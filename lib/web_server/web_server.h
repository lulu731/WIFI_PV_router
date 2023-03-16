#ifndef __WEB_SERVER_H__
#define __WEB_SERVER_H__

#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>

class WEBSERVER
{
  public:
  WEBSERVER();
  ~WEBSERVER();
  void  Start( const String* aLastJson );
  void  HandleClient();
  void  ResetDivEnergy();
  float UpdateDivEnergy( float );
  void  HeaterCmd( const int );
  void  UpdateFirmware();
  void  BroadcastTXT( const String& );

  private:
  ESP8266WebServer* m_Server;
  WebSocketsServer* m_WebSocketServer;
  float             m_DivertedEnergy;
};

#endif