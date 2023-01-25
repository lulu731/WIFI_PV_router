#include "web_socket.h"
#include "web_server.h"

WebSocketsServer WebSocket = WebSocketsServer(81);


void wsEvent(uint8_t num, WStype_t type, uint8_t * message, size_t length)
{
  switch (type) {
    case WStype_TEXT :
    {
      if (message[0] == '0') {
        HandleHeaterCmd(0);
        break;
      }
      else if (message[0] == '1') {
        HandleHeaterCmd(1);
        break;
      }
      else if (message[0] == '9') {
        HandleUpdate();
        break;
      }
      else if (message[0] == '8') {
        HandleResetDivEnerg();
      }
    }
    default: break;
  }
}


void StartWebSocket()
{
  WebSocket.begin();
  WebSocket.onEvent(wsEvent);
}


void WebSocketLoop()
{
  WebSocket.loop();
}


void WebSocketBroadcastTXT(String str)
{
  WebSocket.broadcastTXT(str.c_str(), str.length());
}