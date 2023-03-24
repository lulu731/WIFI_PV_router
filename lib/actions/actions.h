#ifndef _ACTIONS_H_
#define _ACTIONS_H_

#include "actions_type.h"
#include "web_server_itf.h"
#include <stdint.h>
#include <map>

class ACTION
{
  protected:
  WEBSERVER_ITF* m_WebServer;

  public:
  ACTION( WEBSERVER_ITF* aWebServer );
  virtual void Execute( const ACTION_TYPE aActionType ){};
};

class ACTION_HEATER : public ACTION
{
  public:
  ACTION_HEATER( WEBSERVER_ITF* aWebServer );
  void Execute( const ACTION_TYPE aActionType );
};

class ACTION_RESET_DIV_ENERGY : public ACTION
{
  public:
  ACTION_RESET_DIV_ENERGY( WEBSERVER_ITF* aWebServer );
  void Execute( const ACTION_TYPE aActionType );
};

class ACTION_UPDATE_FIRMWARE : public ACTION
{
  public:
  ACTION_UPDATE_FIRMWARE( WEBSERVER_ITF* aWebServer );
  void Execute( const ACTION_TYPE aActionType );
};

class ACTIONS
{
  private:
  WEBSERVER_ITF*                 m_WebServer;
  std::map<ACTION_TYPE, ACTION*> m_Actions;

  public:
  ACTIONS( WEBSERVER_ITF* aWebServer );
  ~ACTIONS();
  void Execute( const ACTION_TYPE aActionType );
};

#endif