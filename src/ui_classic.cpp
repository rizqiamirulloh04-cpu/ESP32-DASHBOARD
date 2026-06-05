#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include "ui_classic.h"

// Pin Hardware
#define TFT_BL 22
#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS   14
#define TFT_DC   15
#define TFT_RST  21
#define STEER_PIN    1
#define THROTTLE_PIN 2

// DEFINISI GLOBAL (Hanya ada di main.cpp)
Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, GFX_NOT_DEFINED);
Arduino_GFX *gfx = new Arduino_ST7789(bus, TFT_RST, 1, true, 172, 320, 34, 0, 34, 0);
Arduino_Canvas *canvas = new Arduino_Canvas(320, 172, gfx);

void setup() {
    // Isolasi Pin Receiver agar tidak mengganggu Booting
    pinMode(STEER_PIN, INPUT_PULLUP);
    pinMode(THROTTLE_PIN, INPUT_PULLUP);

    Serial.begin(115200);
    ledcAttach(TFT_BL, 5000, 8);
    ledcWrite(TFT_BL, 150); 

    gfx->begin();
    gfx->invertDisplay(false);
    gfx->setRotation(1); 
    
    canvas->begin();
    canvas->fillScreen(0x0000); // Black

    // Jeda 1 detik agar hardware stabil sebelum receiver diproses
    delay(1000); 
}

void loop() {
    int rawSteerPWM = pulseIn(STEER_PIN, HIGH, 20000);
    int rawThrottlePWM = pulseIn(THROTTLE_PIN, HIGH, 20000);

    // ... Logika loop dan gambar canvas Anda ...

    gfx->draw16bitRGBBitmap(0, 0, canvas->getFramebuffer(), 320, 172);
    delay(3);
}
