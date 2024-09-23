#ifndef WEBSOCKETCONNECTION_H
#define WEBSOCKETCONNECTION_H

#include <WebSocketsClient.h>

extern WebSocketsClient webSocket; 

void connectWebSocket();

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);

#endif
