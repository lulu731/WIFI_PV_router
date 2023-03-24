#include "actions.h"

ACTION::ACTION( WEBSERVER_ITF* aWebServer ) : m_WebServer( aWebServer )
{
}

ACTION_HEATER::ACTION_HEATER( WEBSERVER_ITF* aWebServer ) : ACTION( aWebServer )
{
}

void ACTION_HEATER::Execute( const ACTION_TYPE aActionType )
{
  m_WebServer->HeaterCmd( aActionType );
}

ACTION_RESET_DIV_ENERGY::ACTION_RESET_DIV_ENERGY( WEBSERVER_ITF* aWebServer ) : ACTION( aWebServer )
{
}

void ACTION_RESET_DIV_ENERGY::Execute( const ACTION_TYPE aActionType )
{
  m_WebServer->ResetDivEnergy();
}

ACTION_UPDATE_FIRMWARE::ACTION_UPDATE_FIRMWARE( WEBSERVER_ITF* aWebServer ) : ACTION( aWebServer )
{
}

void ACTION_UPDATE_FIRMWARE::Execute( const ACTION_TYPE aActionType )
{
  m_WebServer->UpdateFirmware();
}

ACTIONS::ACTIONS( WEBSERVER_ITF* aWebServer ) : m_WebServer( aWebServer )
{
  m_Actions[ACTION_HEATER_OFF_T] = new ACTION_HEATER( m_WebServer );
  m_Actions[ACTION_HEATER_ON_T] = new ACTION_HEATER( m_WebServer );
  m_Actions[ACTION_RESET_DIV_ENERGY_T] = new ACTION_RESET_DIV_ENERGY( aWebServer );
  m_Actions[ACTION_UPDATE_FIRMWARE_T] = new ACTION_UPDATE_FIRMWARE( aWebServer );
}

ACTIONS::~ACTIONS()
{
  delete m_Actions[ACTION_HEATER_OFF_T];
  delete m_Actions[ACTION_HEATER_ON_T];
  delete m_Actions[ACTION_RESET_DIV_ENERGY_T];
  delete m_Actions[ACTION_UPDATE_FIRMWARE_T];
}

void ACTIONS::Execute( const ACTION_TYPE aActionType )
{
  m_Actions[aActionType]->Execute( aActionType );
}