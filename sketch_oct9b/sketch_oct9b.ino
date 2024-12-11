#include <Adafruit_NeoPixel.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// Forward declaration
void handleCommand(String command);

// LED settings
#define LED_PIN 17
#define NUM_LEDS 50
#define LED_BRIGHTNESS 50

// BLE service and characteristic UUIDs
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLEServer* pServer = nullptr;
BLECharacteristic* pCharacteristic = nullptr;
bool deviceConnected = false;

// LED strip object
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Connection callback class
class ServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
        Serial.println("Device connected");
    };

    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
        Serial.println("Device disconnected");
        // Restart advertising when disconnected
        BLEDevice::startAdvertising();
    }
};

// Characteristic callback class
class CharacteristicCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        String command;
        // Get the raw value
        uint8_t* data = pCharacteristic->getData();
        if(pCharacteristic->getLength() > 0) {
            // Convert raw data to String
            command = String((char*)data);
            Serial.println("Received command: " + command);
            handleCommand(command);
        }
    }
};

void clearLEDs() {
    strip.clear();
    strip.show();
}

void parseRouteCommand(String command) {
    clearLEDs();
    command = command.substring(6);  // Remove "ROUTE:" prefix

    while (command.length() > 0) {
        int semiColonIndex = command.indexOf(';');
        String segment;
        
        if (semiColonIndex != -1) {
            segment = command.substring(0, semiColonIndex);
            command = command.substring(semiColonIndex + 1);
        } else {
            segment = command;
            command = "";
        }

        int firstColon = segment.indexOf(':');
        int secondColon = segment.indexOf(':', firstColon + 1);
        int thirdColon = segment.indexOf(':', secondColon + 1);

        if (firstColon != -1 && secondColon != -1 && thirdColon != -1) {
            int pos = segment.substring(0, firstColon).toInt();
            int r = segment.substring(firstColon + 1, secondColon).toInt();
            int g = segment.substring(secondColon + 1, thirdColon).toInt();
            int b = segment.substring(thirdColon + 1).toInt();

            if (pos >= 0 && pos < NUM_LEDS) {
                strip.setPixelColor(pos, strip.Color(r, g, b));
                Serial.printf("Setting LED %d to RGB(%d,%d,%d)\n", pos, r, g, b);
            }
        }
    }
    
    strip.show();
}

void handleCommand(String command) {
    if (command.startsWith("ROUTE:")) {
        parseRouteCommand(command);
    } else if (command == "RESET") {
        clearLEDs();
        Serial.println("Resetting LEDs");
    } else if (command == "STATUS") {
        String status = "Status: OK\nConnected: " + String(deviceConnected);
        Serial.println(status);
        if (deviceConnected) {
            pCharacteristic->setValue(status.c_str());
            pCharacteristic->notify();
        }
    } else {
        Serial.println("Unknown command: " + command);
        clearLEDs();
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("Starting Moonboard BLE Server...");

    // Initialize BLE
    BLEDevice::init("MoonBoard");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());
    
    // Create BLE Service
    BLEService *pService = pServer->createService(SERVICE_UUID);
    
    // Create BLE Characteristic
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE |
        BLECharacteristic::PROPERTY_NOTIFY
    );
    
    pCharacteristic->setCallbacks(new CharacteristicCallbacks());
    pCharacteristic->addDescriptor(new BLE2902());
    
    // Start the service
    pService->start();
    
    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    
    Serial.println("BLE server ready, waiting for connections...");

    // Initialize LED strip
    strip.begin();
    strip.setBrightness(LED_BRIGHTNESS);
    strip.show();
    Serial.println("LED strip initialized");
    
    clearLEDs();
}

void loop() {
    // Monitor connection status
    static unsigned long lastCheck = 0;
    if (millis() - lastCheck > 5000) {
        if (deviceConnected) {
            Serial.println("Client connected");
        } else {
            Serial.println("Waiting for connection...");
        }
        lastCheck = millis();
    }

    // Handle Serial commands (for debugging)
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        handleCommand(command);
    }
}