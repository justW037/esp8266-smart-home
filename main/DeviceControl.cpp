#include "DeviceControl.h"
#include <Arduino.h>

const uint8_t LED_PINS[] = {D0, D1, D2};
const int NUM_LEDS = sizeof(LED_PINS) / sizeof(LED_PINS[0]);

void initDeviceControl() {
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW); 
  }
}


uint8_t getPortFromString(const char* portStr) {
  if (strcmp(portStr, "D0") == 0) return D0;
  if (strcmp(portStr, "D1") == 0) return D1;
  if (strcmp(portStr, "D2") == 0) return D2;
  return 255; 
}

int getPortIndex(uint8_t port) {
   
  for (int i = 0; i < NUM_LEDS; i++) {
    if (port == LED_PINS[i]) {
      return i; 
    }
  }
  return -1;
}

void turnOnDevice(uint8_t port) {
  int index = getPortIndex(port);
  if (index != -1) {
    digitalWrite(LED_PINS[index], HIGH); 
    Serial.printf("LED on port %d turned ON\n", port);
  } else {
    Serial.println("Invalid port name");
  }
}

void turnOffDevice(uint8_t port) {
  int index = getPortIndex(port);
  if (index != -1) {
    digitalWrite(LED_PINS[index], LOW); 
    Serial.printf("LED on port %d turned OFF\n", port);
  } else {
    Serial.println("Invalid port name");
  }
}
