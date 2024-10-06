#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <ArduinoJson.h>
#include <Arduino.h>
#include <LittleFS.h>
#include <TimeLib.h>
#include "DeviceControl.h" 
#include "WebSocketConnection.h"
#include <map> 


void saveOrUpdateMessage(DynamicJsonDocument &newDoc);
void saveOrUpdateCommand(DynamicJsonDocument &newDoc);
void checkAndExecuteSchedule();
DynamicJsonDocument readCommandsFromFile(const char* filename);

struct DeviceState {
    bool isOn;
    time_t lastActionTime; 
};

#endif
