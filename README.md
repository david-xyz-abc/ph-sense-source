# Sensor tests

Quick hardware test sketches, built and flashed with `arduino-cli`.

| Sketch | Board | What it does |
|---|---|---|
| `heartrate_test/` | ESP32 | MAX30102 heart-rate test (I2C on SDA=18, SCL=19) |
| `i2c_diag/` | ESP32 | I2C line-level check + bus scan on D4/D15 |
| `volt_check/` | ESP32 | Reads voltage on D4/D15 |
| `ph_test/` | NodeMCU ESP8266 | Reads ADIY pH board output on A0 |
| `ph_web/` | NodeMCU ESP8266 | Live pH web UI over WiFi at `http://ph.local` |

### Setting up `ph_web` (Windows, Arduino IDE)

1. Install the [Arduino IDE](https://www.arduino.cc/en/software).
2. File -> Preferences -> "Additional boards manager URLs", add
   `https://arduino.esp8266.com/stable/package_esp8266com_index.json`
3. Tools -> Board -> Boards Manager, install **esp8266**.
4. If Windows doesn't detect the board, install the CH340 USB driver.
5. Open `ph_web/ph_web.ino` and replace `your-wifi-ssid` / `your-wifi-password`
   with your WiFi details (2.4 GHz network).
6. Tools -> Board: **NodeMCU 1.0 (ESP-12E Module)**, Tools -> Port: the board's COM port.
7. Click Upload. Open Serial Monitor at 115200 baud to see the board's IP address.
8. On the same WiFi, open `http://ph.local` or that IP in a browser.

Make sure nothing is connected to D4 while uploading.
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
