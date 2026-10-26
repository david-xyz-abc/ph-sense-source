// Quick MAX30102 test for ESP32
// Library: "SparkFun MAX3010x Pulse and Proximity Sensor Library" (Arduino Library Manager)
// Wiring: VIN->3V3, GND->GND, SDA->GPIO18, SCL->GPIO19

#include <Wire.h>
#include "MAX30105.h"   // SparkFun lib uses this name for MAX30102 too
#include "heartRate.h"

MAX30105 sensor;

const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
int beatAvg = 0;

void setup() {
  Serial.begin(115200);
  delay(500);
  Wire.begin(18, 19);  // SDA, SCL

  // Step 1: is anything on the I2C bus? MAX30102 should show up at 0x57
  Serial.println("Scanning I2C...");
  bool found = false;
  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.printf("  Device found at 0x%02X\n", addr);
      found = true;
    }
  }
  if (!found) Serial.println("  Nothing found - check wiring!");

  // Step 2: talk to the sensor
  if (!sensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30102 NOT found. Check wiring/power.");
    while (1) delay(1000);
  }
  Serial.println("MAX30102 OK! Place your finger on the sensor.");

  sensor.setup();                    // default config
  sensor.setPulseAmplitudeRed(0x0A); // red LED low = proof it's alive
  sensor.setPulseAmplitudeGreen(0);
}

void loop() {
  long ir = sensor.getIR();

  if (checkForBeat(ir)) {
    long delta = millis() - lastBeat;
    lastBeat = millis();
    float bpm = 60 / (delta / 1000.0);
    if (bpm > 20 && bpm < 255) {
      rates[rateSpot++] = (byte)bpm;
      rateSpot %= RATE_SIZE;
      beatAvg = 0;
      for (byte i = 0; i < RATE_SIZE; i++) beatAvg += rates[i];
      beatAvg /= RATE_SIZE;
    }
  }

  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 200) {
    lastPrint = millis();
    Serial.printf("IR=%ld  BPM(avg)=%d%s\n", ir, beatAvg,
                  ir < 50000 ? "  (no finger)" : "");
  }
}
