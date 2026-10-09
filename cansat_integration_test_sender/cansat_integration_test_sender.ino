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

#define GPIO2 2
#define GPIO3 3
#define GPIO4 4
#define THD 100

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

// Returns csv string without newline
static String debugRecorder() {
  counter++;
  char csv[23]; // includes null-terminator
  if (snprintf(csv, sizeof(csv), "%d, %d, %d, %d, %d", counter, (uint32_t)millis(), adc_a, adc_b, adc_c) < sizeof(csv)) {
    Serial.printf("[Debug]: Error on debugRecorder\n");
  }
  return String(csv);
}

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
  if (!isReleased) {
    if ((adc_a <= THD && adc_b <= THD) || (adc_b <= THD && adc_c <= THD) || (adc_c <= THD && adc_a <= THD)) {
      isReleased = true;
      rPrintlnDebug("Release detected!");
    }
  }

  // Send CSV data
  if (isReleased) {
    rPrintlnDebug(debugRecorder().c_str());
  }

  delay(1);  // WDT
}
