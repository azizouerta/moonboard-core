# MoonBoard Core

This project is the core software for controlling the MoonBoard LED system using an ESP32. It supports both Bluetooth and USB communication to light predefined routes of LEDs.

## Features
- Control LED routes via Bluetooth or USB Serial.
- Supports two-way communication for testing purposes.
- Routes can be defined and controlled via specific commands (e.g., `ROUTE_1`, `ROUTE_2`).

## Commands
- `ROUTE_1`: Lights the first predefined route of LEDs.
- `ROUTE_2`: Lights the second predefined route of LEDs.
- `RESET`: Turns off all LEDs.

## How to Test
### Via USB
1. Upload the code to the ESP32 using the Arduino IDE.
2. Open the Serial Monitor.
3. Send commands like `ROUTE_1` or `RESET` to test the LED behavior.

### Via Bluetooth
1. Use a Bluetooth terminal app on your phone.
2. Connect to the ESP32 (name: `ESP32_LED_Controller`).
3. Send the same commands (`ROUTE_1`, `RESET`, etc.) via Bluetooth.

## Prerequisites
- ESP32
- WS2811 or WS2812B LED strip
- Adafruit NeoPixel library
- Bluetooth terminal app (for testing Bluetooth communication)
