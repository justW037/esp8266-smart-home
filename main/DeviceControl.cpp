#include "DeviceControl.h"
#include <Arduino.h>
#include "WebSocketConnection.h"
#include <DHT.h>
#include <ArduinoJson.h>

#define DHTPIN D4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);


const uint8_t LED_PINS[] = {D0, D1, D2};
const int NUM_LEDS = sizeof(LED_PINS) / sizeof(LED_PINS[0]);

void initDeviceControl() {
  dht.begin();


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

void sendSensorData() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  const float MAX_TEMPERATURE = 40.0;
  const float MAX_HUMIDITY = 70.0; 

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  DynamicJsonDocument doc(256);
  doc["device_id"] = "DHT11";
  doc["temperature"] = temperature;
  doc["humidity"] = humidity;

  String jsonString;
  serializeJson(doc, jsonString);


  webSocket.sendTXT(jsonString);

  
  Serial.printf("Sent sensor data: %s\n", jsonString.c_str());


  if (temperature > MAX_TEMPERATURE) {
    String temperatureWarning = "warning: Temperature is too high, turn on the cooling device, temperature: " +  String(temperature) + "°C";
    webSocket.sendTXT(temperatureWarning);
    Serial.printf("Sent warning: %s\n", temperatureWarning);
  }
  if (humidity > MAX_HUMIDITY) {
    String humidityWarning = "Warning: Humidity is too high. Consider using a dehumidifier. Current humidity: " + String(humidity) + "%";
    webSocket.sendTXT(humidityWarning);
    Serial.printf("Sent warning: %s\n", humidityWarning);
  }

}
