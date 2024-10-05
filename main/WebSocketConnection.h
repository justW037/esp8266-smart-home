#ifndef WEBSOCKETCONNECTION_H
#define WEBSOCKETCONNECTION_H

#include <WebSocketsClient.h>
#include <ArduinoJson.h>

extern WebSocketsClient webSocket; 

void connectWebSocket();

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);

void saveOrUpdateMessage(DynamicJsonDocument &newDoc);

void printMessages();
#endif
