#include "WiFiConnection.h"

ESP8266WiFiMulti WiFiMulti;     

const char* ssid1 = "Nha 17E 2.4G"; 
const char* password1 = "0389349639"; 

const char* ssid2 = "Another_Network";
const char* password2 = "Another_Password";

void startWiFi() {
  WiFiMulti.addAP(ssid1, password1);

  Serial.print("Connecting to WiFi...");

  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nConnection established!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); 
}
