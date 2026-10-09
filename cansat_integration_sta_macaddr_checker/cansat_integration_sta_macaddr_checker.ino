#include <Arduino.h>
#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.setChannel(1, WIFI_SECOND_CHAN_NONE);
  Serial.print("Starting WiFi in Station mode");
  while (!(WiFi.STA.started())) {
    Serial.print(".");
    delay(100);
  }
  Serial.println("");

  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
}

void loop() {
  delay(100);
}
