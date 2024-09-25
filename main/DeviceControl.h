#ifndef DEVICECONTROL_H
#define DEVICECONTROL_H

#include <Arduino.h>

void initDeviceControl();
uint8_t getPortFromString(const char* portStr);
int getPortIndex(uint8_t port);
void turnOnDevice(uint8_t port);
void turnOffDevice(uint8_t port);


#endif
