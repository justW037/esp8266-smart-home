#include "DeviceControl.h"
#include <Arduino.h>

const int LED_PIN = D0; 

void initDeviceControl() {
  pinMode(LED_PIN, OUTPUT);           
  digitalWrite(LED_PIN, LOW);      
}

void turnOnDevice() {
  digitalWrite(LED_PIN, HIGH);       
  Serial.println("LED turned ON");
}

void turnOffDevice() {
  digitalWrite(LED_PIN, LOW);        
  Serial.println("LED turned OFF");
}
