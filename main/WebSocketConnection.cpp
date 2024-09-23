#include "WebSocketConnection.h"
#include "DeviceControl.h"

WebSocketsClient webSocket;
const char* home_ip = "629c7e8a-a4cb-449f-bdfc-a4ccf71b010c"; 

void connectWebSocket() {
  webSocket.begin("exciting-trout-barely.ngrok-free.app", 80, "/api/v1/ws");

  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000); 
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      break;

    case WStype_CONNECTED: {
      Serial.printf("[WSc] Connected to url: %s\n", payload);
      String tokenMessage = String("token:") + home_ip;
      webSocket.sendTXT(tokenMessage);
      break;
    }

    case WStype_TEXT: {
      Serial.printf("[WSc] get text: %s\n", payload);
      String message = String((char*)payload);

      if (message.equals("command: on")) {
        turnOnDevice();
      } else if (message.equals("command: off")) {
        turnOffDevice();
      }
      break;
    }

    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
      break;
  }
}
