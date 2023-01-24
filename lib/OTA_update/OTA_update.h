#ifndef __OTA_UPDATE_H__
#define __OTA_UPDATE_H__

#include <ESP8266WebServer.h>
void OTAUpdate();
void OTAUpdate(ESP8266WebServer& aServer);
void HandleOTAUpdate();

#endif
