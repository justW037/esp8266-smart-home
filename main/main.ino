#include "WiFiConnection.h" 
#include "WebSocketConnection.h" 
#include "DeviceControl.h" 

void setup() {
  Serial.begin(115200); 

  initDeviceControl(); 
  startWiFi();        
  connectWebSocket();  
}

void loop() {
  webSocket.loop();  
}
