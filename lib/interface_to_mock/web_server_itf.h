#ifndef _WEB_SERVER_ITF_H_
#define _WEB_SERVER_ITF_H_

class WEBSERVER_ITF
{
  public:
  virtual void  ResetDivEnergy() = 0;
  virtual float UpdateDivEnergy( float ) = 0;
  virtual void  HeaterCmd( const int ) = 0;
  virtual void  UpdateFirmware() = 0;
};

#endif