#define GPIO2 2
#define GPIO3 3
#define GPIO4 4

// adc value
int d0 = 0;
int d1 = 0;
int d2 = 0;

void setup() {
  Serial.begin(115200);
  pinMode(GPIO2, INPUT);
  pinMode(GPIO3, INPUT);
  pinMode(GPIO4, INPUT);
}

void loop() {
  // high lux: low value(mV)
  // low lux: high value(mV)
  // D0 pin = GPIO2, D1 pin = GIPO3, D2 pin = GIPO4
  d0 = analogRead(GPIO2);
  d1 = analogRead(GPIO3);
  d2 = analogRead(GPIO4);

  // debug
  String s = String(millis(), DEC);
  s.concat(", ");
  s.concat(String(d0, DEC));
  s.concat(", ");
  s.concat(String(d1, DEC));
  s.concat(", ");
  s.concat(String(d2, DEC));
  Serial.println(s);

  delay(100);
}