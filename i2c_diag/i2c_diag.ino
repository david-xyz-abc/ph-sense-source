// I2C diagnostic: checks line levels and scans with different settings
#include <Wire.h>

void lineLevels(int sda, int scl) {
  pinMode(sda, INPUT_PULLDOWN);
  pinMode(scl, INPUT_PULLDOWN);
  delay(10);
  Serial.printf("  Lines vs pulldown (sensor pull-ups): SDA=%d SCL=%d\n",
                digitalRead(sda), digitalRead(scl));
  pinMode(sda, INPUT);
  pinMode(scl, INPUT);
  delay(10);
  Serial.printf("  Lines floating:                     SDA=%d SCL=%d\n",
                digitalRead(sda), digitalRead(scl));
}

void scan(int sda, int scl, uint32_t freq) {
  Wire.end();
  Wire.begin(sda, scl, freq);
  pinMode(sda, INPUT_PULLUP);  // add internal pull-ups as a backup
  pinMode(scl, INPUT_PULLUP);
  Wire.begin(sda, scl, freq);
  int n = 0;
  for (byte a = 1; a < 127; a++) {
    Wire.beginTransmission(a);
    if (Wire.endTransmission() == 0) {
      Serial.printf("  FOUND 0x%02X (SDA=%d SCL=%d %luHz)\n", a, sda, scl, freq);
      n++;
    }
  }
  if (!n) Serial.printf("  nothing (SDA=%d SCL=%d %luHz)\n", sda, scl, freq);
}

void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.println("=== I2C DIAG ===");
  lineLevels(4, 15);
  scan(4, 15, 100000);
  scan(4, 15, 10000);
  scan(15, 4, 100000);  // in case SDA/SCL are swapped
  Serial.println("=== DONE ===");
  delay(2000);
}
