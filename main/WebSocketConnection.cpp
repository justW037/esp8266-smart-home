#include "WebSocketConnection.h"
#include "DeviceControl.h"
#include <ArduinoJson.h>
#include <Arduino.h>
#include <LittleFS.h>

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
      DynamicJsonDocument doc(1024);
      DeserializationError error = deserializeJson(doc, payload);

      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }

      if (doc.containsKey("action")) {
        saveOrUpdateMessage(doc);
         break; 
      }


      const char* command = doc["command"];
      const char* device_id = doc["device_id"];
      const char* portStr = doc["port"];
      uint8_t port = getPortFromString(portStr);
        
        if (strcmp(command, "on") == 0) {
          turnOnDevice(port);
        } else if (strcmp(command, "off") == 0) {
          turnOffDevice(port);
        }
      break; 

    }

    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
      break;
  }
}

void saveOrUpdateMessage(DynamicJsonDocument &newDoc) {
    // Mở tệp để đọc dữ liệu hiện có
    File file = LittleFS.open("/messages.json", "r");
    DynamicJsonDocument doc(2048);

    // Nếu tệp không tồn tại, tạo tệp mới và thêm message
    if (!file) {
        Serial.println("File not found. Creating a new one with an empty array.");
        JsonArray arr = doc.to<JsonArray>(); // Tạo một mảng rỗng trong doc
        JsonObject newObj = arr.createNestedObject(); // Tạo một object mới trong mảng
        newObj["device_id"] = newDoc["device_id"];
        newObj["device_port"] = newDoc["device_port"];
        newObj["action"] = newDoc["action"];
        newObj["time"] = newDoc["time"];
        newObj["days"] = newDoc["days"];
        newObj["isTurnOn"] = newDoc["isTurnOn"];

        // Ghi mảng mới vào tệp
        file = LittleFS.open("/messages.json", "w");
        serializeJson(doc, file);
        file.close();
        Serial.println("Message saved in new messages.json");
        return; 
    }

    // Đọc dữ liệu JSON từ tệp
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }

    // Tìm kiếm trong mảng để xem có message trùng không
    JsonArray arr = doc.as<JsonArray>();
    bool updated = false;

    for (JsonObject obj : arr) {
        // Kiểm tra nếu device_id và action giống nhau
        if (obj["device_id"] == newDoc["device_id"] && obj["action"] == newDoc["action"]) {
            // Cập nhật dữ liệu
            obj["time"] = newDoc["time"];
            obj["days"] = newDoc["days"];
            obj["isTurnOn"] = newDoc["isTurnOn"];
            updated = true;
            break;
        }
    }

    // Nếu không tìm thấy, thêm mới vào mảng
    if (!updated) {
        JsonObject newObj = arr.createNestedObject();
        newObj["device_id"] = newDoc["device_id"];
        newObj["device_port"] = newDoc["device_port"];
        newObj["action"] = newDoc["action"];
        newObj["time"] = newDoc["time"];
        newObj["days"] = newDoc["days"];
        newObj["isTurnOn"] = newDoc["isTurnOn"];
    }

    // Ghi lại dữ liệu vào tệp
    file = LittleFS.open("/messages.json", "w");
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }
    serializeJson(doc, file);
    file.close();
    Serial.println("Message saved/updated in messages.json");
    // printMessages();
}



void printMessages() {
    // Mở tệp để đọc dữ liệu
    File file = LittleFS.open("/messages.json", "r");
    
    if (!file) {
        Serial.println("File not found. Cannot read messages.");
        return;
    }

    // Đọc dữ liệu JSON từ tệp
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }

    // In ra nội dung của tệp
    serializeJsonPretty(doc, Serial); // In ra nội dung JSON đẹp
    Serial.println(); // Xuống dòng sau khi in
}


