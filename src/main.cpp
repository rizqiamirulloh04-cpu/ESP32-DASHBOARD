#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include "ui_classic.h"

// Definisi Pin
#define TFT_BL 22
// ... (tambahkan definisi pin lainnya)

// DEFINISI VARIABEL (Hanya di satu tempat ini)
Arduino_DataBus *bus = new Arduino_ESP32SPI(15, 14, 7, 6, GFX_NOT_DEFINED);
Arduino_GFX *gfx = new Arduino_ST7789(bus, 21, 1, true, 172, 320, 34, 0, 34, 0);
Arduino_Canvas *canvas = new Arduino_Canvas(320, 172, gfx);

void setup() {
    Serial.begin(115200);
    // ... setup lainnya
    gfx->begin();
    canvas->begin();
    delay(1000);
}

void loop() {
    // Panggil fungsi dari ui_classic.h di sini
    draw_ui_classic(10, 20, 89, -67, 38, true);
    gfx->draw16bitRGBBitmap(0, 0, canvas->getFramebuffer(), 320, 172);
}
