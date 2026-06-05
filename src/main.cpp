#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// PIN DEFINITION (Waveshare ESP32-C6 1.47")
#define TFT_BL 22
#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS   14
#define TFT_DC   15
#define TFT_RST  21

// Gunakan bus standar
Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, GFX_NOT_DEFINED);

// KITA TES DENGAN DRIVER GC9A01 (Banyak unit Waveshare 1.47" menggunakan ini)
Arduino_GFX *gfx = new Arduino_GC9A01(bus, TFT_RST, 0, true);

void setup() {
    // 1. Paksa Backlight menyala penuh
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
    
    // 2. Beri waktu hardware untuk "bangun"
    delay(1000);

    // 3. Inisialisasi layar
    if (gfx->begin()) {
        // Jika inisialisasi berhasil, isi layar dengan warna untuk tes
        gfx->fillScreen(0xF800); // Merah
        delay(1000);
        gfx->fillScreen(0x07E0); // Hijau
        delay(1000);
        gfx->fillScreen(0x001F); // Biru
    }
}

void loop() {
    // Kosongkan agar kita tahu jika ada crash
}
