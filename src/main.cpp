#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include "ui_classic.h"

// --- KONFIGURASI HARDWARE WAVESHARE ESP32-C6 1.47" ---
#define TFT_BL 22
#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS 14
#define TFT_DC 15
#define TFT_RST 21

// Inisialisasi Bus SPI
Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, GFX_NOT_DEFINED);

// Inisialisasi Layar
Arduino_GFX *gfx = new Arduino_ST7789(bus, TFT_RST, 3, true, 172, 320, 34, 0, 34, 0);

// --- VARIABEL DATA ---
int speedValue = 0;
int rpmValue = 0;
int batteryPercent = 89;
int signalDbm = -67;
int steerState = 0; 

void setup() {
    Serial.begin(115200);

    // Aktifkan Backlight
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    // Inisialisasi Layar & UI
    if (!gfx->begin()) {
        Serial.println("Gagal menginisialisasi layar!");
    }
    init_ui(gfx);
    
    Serial.println("System Ready - Menampilkan UI...");
}

void loop() {
    static unsigned long blinkTimer = 0;
    static bool blinkState = false;
    if (millis() - blinkTimer > 400) {
        blinkTimer = millis();
        blinkState = !blinkState;
    }

    // Panggil fungsi UI dari file ui_classic.cpp
    draw_ui_classic(speedValue, rpmValue, batteryPercent, signalDbm, steerState, blinkState);

    delay(10);
}
