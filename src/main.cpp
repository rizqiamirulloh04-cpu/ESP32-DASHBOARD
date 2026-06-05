#include <Arduino.h>
#include <Arduino_GFX_Library.h>

#define TFT_BL 22
#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS   14
#define TFT_DC   15
#define TFT_RST  21

Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, GFX_NOT_DEFINED);
// Menggunakan GC9A01 dengan parameter yang lebih spesifik
Arduino_GFX *gfx = new Arduino_GC9A01(bus, TFT_RST, 3 /* rotasi */, true /* IPS */);
Arduino_Canvas *canvas = new Arduino_Canvas(320, 172, gfx);

void setup() {
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    // Wajib ada jeda agar kontroler layar siap menerima data
    delay(500); 

    if (!gfx->begin()) {
        // Jika gagal, layar tidak akan pernah berubah dari warna biru
    }
    
    // Set rotasi dan arah
    gfx->setRotation(3);
    
    // Test: Menggambar kotak putih untuk memastikan komunikasi data jalan
    canvas->begin();
    canvas->fillScreen(0x0000); // Hitam
    canvas->fillRect(50, 50, 100, 50, 0xFFFF); // Kotak putih
    gfx->draw16bitRGBBitmap(0, 0, canvas->getFramebuffer(), 320, 172);
}

void loop() {
    // Tidak perlu diisi dulu, yang penting lihat apakah kotak putih muncul
}
