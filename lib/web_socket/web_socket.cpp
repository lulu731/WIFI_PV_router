#include "web_socket.h"

WebSocketsServer WebSocket = WebSocketsServer(81);


void wsEvent(uint8_t num, WStype_t type, uint8_t * message, size_t length)
{

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