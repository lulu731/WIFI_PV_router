#ifndef __WEB_SERVER_H__
#define __WEB_SERVER_H__

// function prototypes for HTTP handlers
void HandleRoot();
void HandleHeaterCmd();
void HandleUpdate();
void HandleNotFound();

void StartWebserver();
void ServerHandleClient();

#endif