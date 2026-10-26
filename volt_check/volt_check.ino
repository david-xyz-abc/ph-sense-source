// Measures idle voltage on the sensor's SDA/SCL lines
// Temporarily wire: SDA->GPIO4, SCL->GPIO15 (VIN->3V3, GND->GND unchanged)

void setup() {
  Serial.begin(115200);
  delay(500);
  analogSetAttenuation(ADC_11db);  // 0-3.3V range
  pinMode(4, INPUT);
  pinMode(15, INPUT);
}

void loop() {
  Serial.printf("SDA=%.2fV  SCL=%.2fV\n",
                analogReadMilliVolts(4) / 1000.0,
                analogReadMilliVolts(15) / 1000.0);
  delay(500);
}
