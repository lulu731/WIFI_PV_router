#ifndef __NTP_TIME_ITF_H__
#define __NTP_TIME_ITF_H__

class TIME_CLIENT_ITF
{
  public:
  virtual ~TIME_CLIENT_ITF(){};
  virtual bool          Init() = 0;
  virtual unsigned long GetEpochTime() = 0;
};

#endif