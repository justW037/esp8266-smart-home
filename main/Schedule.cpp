#include "Schedule.h"

std::map<int, DeviceState> deviceStates;  

void saveOrUpdateMessage(DynamicJsonDocument &newDoc) {

    File file = LittleFS.open("/messages.json", "r");
    DynamicJsonDocument doc(2048);
    if (!file) {
        Serial.println("File not found. Creating a new one with an empty array.");
        JsonArray arr = doc.to<JsonArray>(); 
        JsonObject newObj = arr.createNestedObject(); 
        newObj["device_id"] = newDoc["device_id"];
        newObj["device_port"] = newDoc["device_port"];
        newObj["action"] = newDoc["action"];
        newObj["time"] = newDoc["time"];
        newObj["days"] = newDoc["days"];
        newObj["isTurnOn"] = newDoc["isTurnOn"];


        file = LittleFS.open("/messages.json", "w");
        serializeJson(doc, file);
        file.close();
        Serial.println("Message saved in new messages.json");
        return; 
    }

    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }


    JsonArray arr = doc.as<JsonArray>();
    bool updated = false;

    for (JsonObject obj : arr) {
        if (obj["device_id"] == newDoc["device_id"] && obj["action"] == newDoc["action"]) {
            obj["time"] = newDoc["time"];
            obj["days"] = newDoc["days"];
            obj["isTurnOn"] = newDoc["isTurnOn"];
            updated = true;
            break;
        }
    }

    if (!updated) {
        JsonObject newObj = arr.createNestedObject();
        newObj["device_id"] = newDoc["device_id"];
        newObj["device_port"] = newDoc["device_port"];
        newObj["action"] = newDoc["action"];
        newObj["time"] = newDoc["time"];
        newObj["days"] = newDoc["days"];
        newObj["isTurnOn"] = newDoc["isTurnOn"];
    }

    file = LittleFS.open("/messages.json", "w");
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }
    serializeJson(doc, file);
    file.close();
    Serial.println("Message saved/updated in messages.json");

}


void saveOrUpdateCommand(DynamicJsonDocument &newDoc) {

    File file = LittleFS.open("/commands.json", "r");
    DynamicJsonDocument doc(2048);
    if (!file) {
        Serial.println("File not found. Creating a new one with an empty array.");
        JsonArray arr = doc.to<JsonArray>(); 
        JsonObject newObj = arr.createNestedObject(); 
        newObj["command"] = newDoc["command"];
        newObj["device_id"] = newDoc["device_id"];
        newObj["port"] = newDoc["port"];
 
        file = LittleFS.open("/commands.json", "w");
        serializeJson(doc, file);
        file.close();
        Serial.println("Message saved in new commands.json");
        return; 
    }

    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }

    JsonArray arr = doc.as<JsonArray>();
    bool updated = false;

    for (JsonObject obj : arr) {
        if (obj["device_id"] == newDoc["device_id"] && obj["port"] == newDoc["port"]) {
            obj["command"] = newDoc["command"];
            updated = true;
            break;
        }
    }

    if (!updated) {
        JsonObject newObj = arr.createNestedObject();
        newObj["command"] = newDoc["command"];
        newObj["device_id"] = newDoc["device_id"];
        newObj["port"] = newDoc["port"];
    }

    file = LittleFS.open("/commands.json", "w");
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }
    serializeJson(doc, file);
    file.close();
    Serial.println("Message saved/updated in commands.json");


}


void checkAndExecuteSchedule() {

    time_t currentTime = now();    
    int currentHour = hour(currentTime);     
    int currentMinute = minute(currentTime);

    File file = LittleFS.open("/messages.json", "r");

    if (!file) {
        Serial.println("File not found. Cannot read messages.");
        return;
    }


    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }


    JsonArray arr = doc.as<JsonArray>();
    for (JsonObject obj : arr) {
        int device_id = obj["device_id"];
        const char* action = obj["action"];
        const char* timeStr = obj["time"];  
        JsonArray daysArray = obj["days"].as<JsonArray>();
        bool isTurnOn = obj["isTurnOn"];
        
        const char* device_port_str = obj["device_port"]; 
        uint8_t port = getPortFromString(device_port_str);
        int scheduledHour, scheduledMinute;
        sscanf(timeStr, "%d:%d", &scheduledHour, &scheduledMinute);
        

        int currentDayOfWeek = weekday() - 1;
        bool isDayMatched = false;
        bool newIsTurnOn = true; 
        for (JsonVariant day : daysArray) {

          int dayInt = day.as<int>(); 
          if (dayInt == currentDayOfWeek) {
              isDayMatched = true;
              break;
          }
          if (dayInt == 7 ) {
              isDayMatched = true;
              newIsTurnOn = false;
              break;
          }
        }

        if (isDayMatched && scheduledHour == currentHour && scheduledMinute == currentMinute) {  

            if (deviceStates.find(device_id) == deviceStates.end()) {
                deviceStates[device_id] = {false, 0};
            }

            DeviceState &deviceState = deviceStates[device_id];
            if (currentTime - deviceState.lastActionTime < 60) {
                continue; 
            }

            String command = "";
            if (strcmp(action, "turnOn") == 0 && isTurnOn) {
                turnOnDevice(port);  
                command = "on";
                deviceState.isOn = true;
            } else if (strcmp(action, "turnOff") == 0 && isTurnOn) {
                turnOffDevice(port); 
                command = "off";
                deviceState.isOn = true;
            } 
            // else {
            //     Serial.println("No valid action or condition to execute.");
            // }
            if (command.length() > 0) {
                if(!newIsTurnOn){
                  DynamicJsonDocument newMessageDoc(256);
                  newMessageDoc["device_id"] = device_id;
                  newMessageDoc["device_port"] = device_port_str;
                  newMessageDoc["action"] = action; 
                  newMessageDoc["time"] = timeStr;  
                  newMessageDoc["days"] = daysArray; 
                  newMessageDoc["isTurnOn"] = newIsTurnOn; 
                  saveOrUpdateMessage(newMessageDoc);
                  String newMessage;
                  serializeJson(newMessageDoc, newMessage);
                  webSocket.sendTXT(newMessage);
                }
                DynamicJsonDocument messageDoc(256);
                messageDoc["device_id"] = String(device_id);
                messageDoc["port"] = device_port_str;
                messageDoc["command"] = command;
                String message;
                serializeJson(messageDoc, message);
                webSocket.sendTXT(message);
                deviceState.lastActionTime = currentTime;
            }
        } 
        // else {
        //     Serial.println("No matching day or time found, skipping action.");
        // }
    }
}

DynamicJsonDocument readCommandsFromFile(const char* filename) {
    File file = LittleFS.open(filename, "r");
    DynamicJsonDocument doc(2048);

    if (!file) {
        Serial.println("Failed to open commands.json for reading");
        return doc;
    }

    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return doc; 
    }

    return doc;
}




