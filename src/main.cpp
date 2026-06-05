#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// PIN DEFINITION
#define TFT_BL 22
#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS   14
#define TFT_DC   15
#define TFT_RST  21

// DRIVER & CANVAS (Menggunakan GC9A01 sesuai tes sebelumnya)
Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, GFX_NOT_DEFINED);
Arduino_GFX *gfx = new Arduino_GC9A01(bus, TFT_RST, 0, true);
Arduino_Canvas *canvas = new Arduino_Canvas(320, 172, gfx);

// FUNGSI DASAR MENGGAMBAR BUSUR (ARC)
void drawArc(int cx, int cy, int r, int start, int end, uint16_t color) {
    for (int i = start; i < end; i++) {
        float angle = i * PI / 180;
        int x = cx + r * cos(angle);
        int y = cy + r * sin(angle);
        canvas->drawPixel(x, y, color);
    }
}

void setup() {
    // Inisialisasi Backlight
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
    
    // Inisialisasi Layar
    gfx->begin();
    canvas->begin();
    canvas->fillScreen(0x0000); // Background Hitam
}

void loop() {
    // 1. Bersihkan Canvas
    canvas->fillScreen(0x0000);

    // 2. Gambar Speedometer (Busur Sederhana)
    drawArc(160, 85, 80, 150, 390, 0xFFFF); // Busur Putih
    
    // 3. Tulis Angka Speed (Contoh 80 KM/H)
    canvas->setTextSize(3);
    canvas->setCursor(120, 70);
    canvas->setTextColor(0xFFFF);
    canvas->print("80");
    canvas->setTextSize(1);
    canvas->setCursor(150, 100);
    canvas->print("KM/H");

    // 4. Update ke Layar Fisik
    gfx->draw16bitRGBBitmap(0, 0, canvas->getFramebuffer(), 320, 172);
    
    delay(50);
}
