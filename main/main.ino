#include "WiFiConnection.h" 
#include "WebSocketConnection.h" 
#include "DeviceControl.h" 
#include <LittleFS.h>

unsigned long lastSensorSendTime = 0;

void setup() {
  Serial.begin(115200); 

if (!LittleFS.begin()) {
        Serial.println("Failed to mount LittleFS filesystem");
        return;
    }
    //  if (LittleFS.remove("/messages.json")) {
    //     Serial.println("File messages.json deleted successfully");
    // } else {
    //     Serial.println("Failed to delete file messages.json");
    // }
  initDeviceControl(); 
  startWiFi();        
  connectWebSocket();  

}

void loop() {
  webSocket.loop();  

  unsigned long currentMillis = millis();
  if (currentMillis - lastSensorSendTime >= 60000) {
    sendSensorData();
    lastSensorSendTime = currentMillis;
  }
}
