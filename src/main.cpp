#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// 1. DEFINISI PIN (Global)
#define TFT_BL 22
#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS   14
#define TFT_DC   15
#define TFT_RST  21

// 2. DEKLARASI GLOBAL (Agar bisa diakses di setup dan loop)
Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, GFX_NOT_DEFINED);
Arduino_GFX *gfx = new Arduino_ST7789(bus, TFT_RST, 3, true, 172, 320, 34, 0, 34, 0);
Arduino_Canvas *canvas = new Arduino_Canvas(320, 172, gfx);

void setup() {
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    gfx->begin();
    canvas->begin();
}

void loop() {
    // Sekarang 'canvas' dan 'gfx' bisa dikenali karena sudah dideklarasikan di atas
    canvas->fillScreen(0x0000); 
    
    // Test tulis sesuatu
    canvas->setCursor(50, 50);
    canvas->setTextColor(0xFFFF);
    canvas->print("TESTING OK");

    gfx->draw16bitRGBBitmap(0, 0, canvas->getFramebuffer(), 320, 172);
}
