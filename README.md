# Sensor tests

Quick hardware test sketches, built and flashed with `arduino-cli`.

| Sketch | Board | What it does |
|---|---|---|
| `heartrate_test/` | ESP32 | MAX30102 heart-rate test (I2C on SDA=18, SCL=19) |
| `i2c_diag/` | ESP32 | I2C line-level check + bus scan on D4/D15 |
| `volt_check/` | ESP32 | Reads voltage on D4/D15 |
| `ph_test/` | NodeMCU ESP8266 | Reads ADIY pH board output on A0 |
| `ph_web/` | NodeMCU ESP8266 | Live pH web UI over WiFi at `http://ph.local` |

For `ph_web`, copy `ph_web/secrets.h.example` to `ph_web/secrets.h` and fill in your WiFi details (it is gitignored).
The pH value is an uncalibrated estimate until calibrated with buffer solutions.

## pH sensor wiring (NodeMCU)

- pH board O/P `+` -> `A0`
- pH board O/P `-` -> `GND`
- pH board powered from its own 9V supply (white connector)
- Leave the `-5 G +5` header unconnected
- Do not use `D4`: it is a boot pin and blocks booting/uploads when pulled low

## Build / upload

```sh
arduino-cli compile --fqbn esp8266:esp8266:nodemcuv2 ph_test
arduino-cli upload  --fqbn esp8266:esp8266:nodemcuv2 -p /dev/ttyUSB0 ph_test

arduino-cli compile --fqbn esp32:esp32:esp32 heartrate_test
arduino-cli upload  --fqbn esp32:esp32:esp32 -p /dev/ttyUSB0 heartrate_test
```

If upload fails to connect, hold BOOT/FLASH, tap EN/RST, release BOOT/FLASH, and retry.
