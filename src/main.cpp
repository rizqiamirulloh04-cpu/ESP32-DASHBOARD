#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include "ui_classic.h"

// Kita tetap gunakan pin 1 dan 2 sesuai permintaan Anda
#define STEER_PIN 1    
#define THROTTLE_PIN 2 

Arduino_DataBus *bus = new Arduino_ESP32SPI(8, 9, 10, 11, GFX_NOT_DEFINED);
Arduino_GFX *gfx = new Arduino_GC9A01(bus, 12, 0, true);

void setup() {
    // 1. Matikan dulu komunikasi serial untuk membebaskan pin 1 (TX0)
    // Serial.begin(115200); 

    // 2. Inisialisasi layar SEBELUM menyentuh pin sensor
    if (!gfx->begin()) {
        // Jika gagal, tidak perlu print ke Serial
    }
    gfx->fillScreen(0x0000); 
    init_ui(gfx);

    // 3. BARU atur pin sensor setelah layar siap
    pinMode(STEER_PIN, INPUT);
    pinMode(THROTTLE_PIN, INPUT);
}

void loop() {
    int rawSteer = analogRead(STEER_PIN);
    int rawThrottle = analogRead(THROTTLE_PIN);

    // ... (kode mapping dan draw_ui_classic tetap sama)
    static int currentSpeed = 0;
    static int currentRPM = 0;
    currentRPM = (currentRPM * 0.8) + (map(constrain(rawSteer, 0, 4095), 0, 4095, 0, 100) * 0.2);
    currentSpeed = (currentSpeed * 0.8) + (map(constrain(rawThrottle, 0, 4095), 0, 4095, 0, 200) * 0.2);

    draw_ui_classic(currentSpeed, currentRPM, 89, 0, 0, (millis() % 1000 < 500));
    delay(30); 
}
