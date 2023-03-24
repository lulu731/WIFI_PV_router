#include "web_server.h"
#include "actions_type.h"
#include "OTA_update.h"
#include <LittleFS.h>

WEBSERVER::WEBSERVER()
{
  m_Server = new ESP8266WebServer( 80 );
  m_WebSocketServer = new WebSocketsServer( 81 );
  m_Actions = new ACTIONS( this );
}


WEBSERVER::~WEBSERVER()
{
  delete m_Actions;
  delete m_WebSocketServer;
  m_Server->stop();
  delete m_Server;
}


void WEBSERVER::Start( const String* aLastJson )
{
  m_Server->on( "/", HTTP_GET,
                [this]()
                {
                  File   file = LittleFS.open( "/index.html", "r" );
                  String str = file.readString();
                  file.close();
                  m_Server->send( 200, "text/html", str );
                } );

  m_Server->onNotFound(
          [this]()
          {
            m_Server->send( 404, "text/plain", "404: Not found" );
          } );

  m_Server->begin();
  m_WebSocketServer->begin();
  m_WebSocketServer->onEvent(
          [this, aLastJson]( uint8_t num, WStype_t type, uint8_t* message, size_t length )
          {
            switch( type )
            {
            case WStype_CONNECTED: BroadcastTXT( *aLastJson ); break;
            case WStype_TEXT:
            {
              ACTION_TYPE actionType = ACTION_TYPE( int( message[0] ) - 48 );
              this->m_Actions->Execute( actionType );
            }
            default: break;
            }
          } );
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


float WEBSERVER::UpdateDivEnergy( float aDivEnergy )
{
  m_DivertedEnergy += aDivEnergy;
  return m_DivertedEnergy;
}


void WEBSERVER::HeaterCmd( const int order )
{
  Serial.print( order );
}


void WEBSERVER::UpdateFirmware()
{
  OTAUpdate( this );
  for( size_t i = 0; i < 1200; i++ )
  {
    HandleOTAUpdate();
    delay( 100 );
  }
  EndOTAUpdate(); //No update processed after two minutes
}


void WEBSERVER::BroadcastTXT( const String& aStr )
{
  m_WebSocketServer->broadcastTXT( aStr.c_str(), aStr.length() );
}