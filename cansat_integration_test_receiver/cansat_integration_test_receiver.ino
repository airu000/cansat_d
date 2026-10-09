#include <Arduino.h>
#include <ESP32_NOW_Serial.h>
#include <MacAddress.h>
#include <WiFi.h>

// Sender(CanSat): Station mode
// Receiver: Station mode

// Channel to be used by the ESP-NOW protocol
#define ESPNOW_WIFI_CHANNEL 1

// Set the MAC address of the device that will send csv data
const MacAddress peer_mac({0x14, 0x63, 0x93, 0xBE, 0xE0, 0x38});

ESP_NOW_Serial_Class NowSerial(peer_mac, ESPNOW_WIFI_CHANNEL, WIFI_IF_STA);

void setup() {
  Serial.begin(115200);
  
  WiFi.mode(WIFI_STA);
  Serial.printf("[Debug] WiFi Mode: Station\n");
  
  WiFi.setChannel(ESPNOW_WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE);
  Serial.printf("[Debug] Channel: %d\n", ESPNOW_WIFI_CHANNEL);
  
  while (!(WiFi.STA.started())) {
    delay(100);
  }
  Serial.printf("[Debug] WiFi started in Station mode\n");
  Serial.printf("[Debug] MAC Address: %s\n", WiFi.macAddress().c_str());

  // Start the ESP-NOW communication
  NowSerial.begin(115200);
  Serial.printf("[Debug] ESP-NOW communication started\n");
}

void loop() {
  while (NowSerial.available()) {
    Serial.write(NowSerial.read());
  }

  delay(1); // WDT
}
