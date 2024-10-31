#include <Adafruit_NeoPixel.h>
#include <BluetoothSerial.h>  // Bluetooth Serial library

// LED settings
#define LED_PIN 17            // GPIO pin for LED data line
#define NUM_LEDS 50           // Number of LEDs in the strip
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Bluetooth serial object
BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);  // Initialize Serial Monitor
  SerialBT.begin("ESP32_LED_Controller");  // Initialize Bluetooth
  strip.begin();  // Initialize the LED strip
  strip.show();   // Turn off all LEDs initially
  Serial.println("Waiting for commands...");

  clearLEDs();
}

void loop() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    handleCommand(command);
  }

  if (SerialBT.available()) {
    String command = SerialBT.readStringUntil('\n');
    handleCommand(command);
  }
}

void handleCommand(String command) {
  if (command.startsWith("ROUTE:")) {
    parseRouteCommand(command);
  } else if (command == "RESET") {
    clearLEDs();
  } else {
    Serial.println("Unknown command, turning off LEDs...");
    clearLEDs();
  }
}

void parseRouteCommand(String command) {
  clearLEDs();

  command = command.substring(6);  // Remove "ROUTE:" from the start of the command
  int pos, r, g, b;

  while (command.length() > 0) {
    int semiColonIndex = command.indexOf(';');
    String segment = command.substring(0, semiColonIndex);
    if (semiColonIndex != -1) {
      command = command.substring(semiColonIndex + 1);
    } else {
      command = "";  // End of command
    }

    // Split the segment "pos:r:g:b" into individual components
    int firstColon = segment.indexOf(':');
    int secondColon = segment.indexOf(':', firstColon + 1);
    int thirdColon = segment.indexOf(':', secondColon + 1);

    pos = segment.substring(0, firstColon).toInt();
    r = segment.substring(firstColon + 1, secondColon).toInt();
    g = segment.substring(secondColon + 1, thirdColon).toInt();
    b = segment.substring(thirdColon + 1).toInt();

    // Set the LED color
    strip.setPixelColor(pos, strip.Color(r, g, b));
  }

  strip.show();  // Update the LED strip
}

void clearLEDs() {
  strip.clear();
  strip.show();
}
