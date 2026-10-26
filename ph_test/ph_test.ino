// Quick test for ADIY pH sensor board (9V powered, analog output)
// Board: NodeMCU ESP8266 (ESP-12E)
// Wiring: board O/P "+" -> A0, board O/P "-" -> NodeMCU GND
// Board is powered by its own 9V supply (white connector), NOT from the NodeMCU
// NodeMCU's A0 has a built-in divider, so it reads 0-3.3V as 0-1023

const float VREF_MV = 3300.0;

void setup() {
  Serial.begin(115200);
}

void loop() {
  // average a bunch of samples to smooth out noise
  long sum = 0;
  for (int i = 0; i < 50; i++) {
    sum += analogRead(A0);
    delay(2);
  }
  int raw = sum / 50;
  int mv = raw * VREF_MV / 1023;
  Serial.printf("O/P = %d mV (raw %d)%s\n", mv, raw, raw > 1000 ? "  (at limit!)" : "");
  delay(500);
}
