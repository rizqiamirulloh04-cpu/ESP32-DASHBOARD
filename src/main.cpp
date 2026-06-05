#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include "ui_classic.h"

// Konfigurasi Pin pada board ESP32-C6-LCD-1.47
#define STEER_PIN 1    // Pin 1 untuk Steering
#define THROTTLE_PIN 2 // Pin 2 untuk Throttle

// Variabel Global
int currentSpeed = 0;
int currentRPM = 0;
int batteryLevel = 89; 
bool blinkState = false;

// Setup Layar
Arduino_DataBus *bus = new Arduino_ESP32SPI(8, 9, 10, 11, GFX_NOT_DEFINED);
Arduino_GFX *gfx = new Arduino_GC9A01(bus, 12, 0, true);

void setup() {
    Serial.begin(115200);
    
    // Set pin sebagai input analog
    pinMode(STEER_PIN, INPUT);
    pinMode(THROTTLE_PIN, INPUT);
    
    if (!gfx->begin()) {
        Serial.println("gfx->begin() failed!");
    }
    
    // Perbaikan: Mengganti BLACK dengan 0x0000
    gfx->fillScreen(0x0000); 
    
    init_ui(gfx);
}

void loop() {
    // 1. Baca Sensor Analog
    int rawSteer = analogRead(STEER_PIN);
    int rawThrottle = analogRead(THROTTLE_PIN);

    // 2. Mapping nilai sensor (0-4095) ke rentang UI
    int targetRPM = map(constrain(rawSteer, 0, 4095), 0, 4095, 0, 100);
    int targetSpeed = map(constrain(rawThrottle, 0, 4095), 0, 4095, 0, 200);

    // 3. Smoothing agar jarum tidak bergetar
    currentRPM = (currentRPM * 0.8) + (targetRPM * 0.2);
    currentSpeed = (currentSpeed * 0.8) + (targetSpeed * 0.2);

    // 4. Simulasi Blink Sen
    blinkState = (millis() % 1000 < 500);

    // 5. Update UI (Fungsi dari ui_classic.cpp)
    draw_ui_classic(currentSpeed, currentRPM, batteryLevel, 0, 0, blinkState);

    delay(30); 
}
