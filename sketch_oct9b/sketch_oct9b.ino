#include <Adafruit_NeoPixel.h>
#include <BluetoothSerial.h>  // Bluetooth Serial library

// LED settings
#define LED_PIN 17            // GPIO pin for LED data line
#define NUM_LEDS 50           // Number of LEDs in the strip
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Bluetooth serial object
BluetoothSerial SerialBT;

// Callback to notify when a device connects
bool deviceConnected = false;

void setup() {
  Serial.begin(115200);  // Initialize Serial Monitor with 115200 baud rate
  SerialBT.begin("ESP32_LED_Controller");  // Initialize Bluetooth
  SerialBT.register_callback(btCallback);  // Register the callback for Bluetooth

  strip.begin();  // Initialize the LED strip
  strip.show();   // Turn off all LEDs initially
  Serial.println("Waiting for commands...");

  // Set all LEDs off initially
  clearLEDs();
}

void loop() {
  // USB Serial communication
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');  // Read command via USB
    Serial.println("Received command from USB: " + command);  // Debugging
    handleCommand(command);
  }

  // Bluetooth communication
  if (SerialBT.available()) {
    String btCommand = SerialBT.readStringUntil('\n');  // Read command via Bluetooth
    Serial.println("Received command from Bluetooth: " + btCommand);  // Debugging
    handleCommand(btCommand);
  }
}

// Bluetooth callback function
void btCallback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
  if (event == ESP_SPP_SRV_OPEN_EVT) {
    deviceConnected = true;
    Serial.println("Bluetooth device connected.");
  } else if (event == ESP_SPP_CLOSE_EVT) {
    deviceConnected = false;
    Serial.println("Bluetooth device disconnected.");
  }
}

// Function to handle commands
void handleCommand(String command) {
  command.trim(); // Remove any newline characters or extra spaces

  // Clear LEDs first
  clearLEDs();

  if (command == "ROUTE_1") {
    Serial.println("Lighting Route 1 LEDs...");
    // Light the LEDs for route 1 (based on your predefined setup)
    strip.setPixelColor(0, strip.Color(0, 255, 0));  // LED 1 Green
    strip.setPixelColor(20, strip.Color(0, 255, 0)); // LED 20 Green
    strip.setPixelColor(2, strip.Color(255, 0, 0));  // LED 2 Red
    strip.setPixelColor(19, strip.Color(255, 0, 0)); // LED 19 Red
    strip.setPixelColor(17, strip.Color(0, 0, 255)); // LED 17 Blue
    strip.setPixelColor(16, strip.Color(0, 0, 255)); // LED 16 Blue
    strip.setPixelColor(30, strip.Color(0, 0, 255)); // LED 30 Blue

    // Show the LEDs
    strip.show();
  } else if (command == "ROUTE_2") {
    Serial.println("Lighting Route 2 LEDs...");
     // Light the LEDs for route 2 (based on your predefined setup)
    strip.setPixelColor(49, strip.Color(0, 255, 0));  // LED 49 Green
    strip.setPixelColor(48, strip.Color(0, 255, 0));  // LED 48 Green
    strip.setPixelColor(46, strip.Color(255, 0, 0));  // LED 46 Red
    strip.setPixelColor(25, strip.Color(255, 0, 0));  // LED 25 Red
    strip.setPixelColor(26, strip.Color(0, 0, 255));  // LED 26 Blue
    strip.setPixelColor(27, strip.Color(0, 0, 255));  // LED 27 Blue

    // Show the LEDs
    strip.show();
  } else if (command == "RESET") {
    clearLEDs();
  } else {
    Serial.println("Unknown command, turning off LEDs...");
    clearLEDs();
  }
}

// Function to clear all LEDs
void clearLEDs() {
  strip.clear();
  strip.show();
}
