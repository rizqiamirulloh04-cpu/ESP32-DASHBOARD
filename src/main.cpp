#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// ... (Simpan definisi PIN, bus, gfx, dan canvas yang sudah berhasil tadi) ...

// Tambahkan variabel untuk data
volatile int speed = 0;

void setup() {
    // ... (simpan setup yang sudah berhasil tadi) ...
    pinMode(1, INPUT_PULLUP); // Sesuaikan dengan pin receiver Anda
}

void loop() {
    // 1. Baca data dari receiver (Contoh menggunakan pulseIn)
    int rawPulse = pulseIn(1, HIGH, 20000);
    speed = map(rawPulse, 1000, 2000, 0, 100); // Sesuaikan range receiver Anda

    // 2. Gambar Ulang Dashboard
    canvas->fillScreen(0x0000);
    canvas->drawCircle(160, 86, 80, 0x7BEF);
    
    canvas->setCursor(130, 70);
    canvas->setTextSize(4);
    canvas->print(speed);
    
    // 3. Update Layar
    gfx->draw16bitRGBBitmap(0, 0, canvas->getFramebuffer(), 320, 172);
}
