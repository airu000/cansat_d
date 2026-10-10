#include <Arduino.h>
#include <ESP32_NOW_Serial.h>
#include <MacAddress.h>
#include <WiFi.h>

// Sender(CanSat): Station mode
// Receiver: Station mode

// Channel to be used by the ESP-NOW protocol
#define ESPNOW_WIFI_CHANNEL 1

// Set the MAC address of the device that will receive csv data
const MacAddress peer_mac({ 0xAC, 0x27, 0x6E, 0x82, 0xCB, 0x38 });

ESP_NOW_Serial_Class NowSerial(peer_mac, ESPNOW_WIFI_CHANNEL, WIFI_IF_STA);

// -----------------------------------------------------

// Comment out for COUNTER MODE
// #define DEBUG_RECORDER_MODE

// -----------------------------------------------------

#define GPIO2 2
#define GPIO3 3
#define GPIO4 4
#define LDR_THD 100
#define T_THD 30000 // 30s after boot

uint16_t adc_a = 0;
uint16_t adc_b = 0;
uint16_t adc_c = 0;
bool isReleased = false;
uint32_t counter = 0;

// Println remotely with Serial debugging
static void rPrintlnDebug(const char *str) {
  while (NowSerial.availableForWrite() <= strlen(str)) { // strlen + newline
    delay(1);
  }
  NowSerial.printf("%s\n", str);
  Serial.printf("[CanSat] %s\n", str);
}

#ifdef DEBUG_RECORDER_MODE
// Returns csv string without newline
static String debugRecorder() {
  String csv = String(counter);
  csv += ", ";
  csv += String(millis());
  csv += ", ";
  csv += String(adc_a);
  csv += ", ";
  csv += String(adc_b);
  csv += ", ";
  csv += String(adc_c);
  return csv;
}
#endif

void setup() {
  Serial.begin(115200);

  pinMode(GPIO2, INPUT);
  pinMode(GPIO3, INPUT);
  pinMode(GPIO4, INPUT);

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
  // High lux: low value(mV)
  // Low lux: high value(mV)
  adc_a = analogRead(GPIO2);
  adc_b = analogRead(GPIO3);
  adc_c = analogRead(GPIO4);
  Serial.printf("[Debug] isReleased: %s, adc_a: %d, adc_b: %d, adc_c: %d\n", isReleased ? "true" : "false", adc_a, adc_b, adc_c);

  // Release detection
  if (!isReleased && millis() > T_THD) {
    if ((adc_a <= LDR_THD && adc_b <= LDR_THD) || (adc_b <= LDR_THD && adc_c <= LDR_THD) || (adc_c <= LDR_THD && adc_a <= LDR_THD)) {
      isReleased = true;
      rPrintlnDebug("Release detected!");
    }
  }

  // Send CSV data
  if (isReleased) {
    counter++;
#ifdef DEBUG_RECORDER_MODE
    rPrintlnDebug(debugRecorder().c_str());
#else
    rPrintlnDebug(String(counter).c_str());
#endif
  }

  delay(1);  // WDT
}
