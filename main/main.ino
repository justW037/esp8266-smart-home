#include "WiFiConnection.h" 
#include "WebSocketConnection.h" 
#include "DeviceControl.h" 
#include <LittleFS.h>
#include "Schedule.h"
#include <NTPClient.h>      
#include <WiFiUdp.h>         
#include <TimeLib.h>  

unsigned long lastSensorSendTime = 0;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7 * 3600, 60000);

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
  timeClient.begin();
  initDeviceControl(); 
  startWiFi();        
  connectWebSocket();  
  

}

void loop() {
  timeClient.update();
  time_t ntpTime = timeClient.getEpochTime();
  setTime(ntpTime);
  webSocket.loop();  
  checkAndExecuteSchedule();
  unsigned long currentMillis = millis();
  if (currentMillis - lastSensorSendTime >= 60000) {
    sendSensorData();
    lastSensorSendTime = currentMillis;
  }
}
