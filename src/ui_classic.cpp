#include "ui_classic.h"

Arduino_GFX *gfx;
Arduino_Canvas *canvas;

void init_ui(Arduino_GFX *gfx_ptr) {
    gfx = gfx_ptr;
    canvas = new Arduino_Canvas(320, 172, gfx);
    canvas->begin();
}

void draw_ui_classic(int speed, int rpm, int batt, int sig, int steerState, bool blinkState) {
    canvas->fillScreen(0x0000); // Hitam

    // --- 1. LINGKARAN SPEEDOMETER (Kiri) ---
    // Gambar busur utama
    canvas->drawArc(100, 85, 75, 5, 140, 400, 0x0010); // Dark Blue Ring
    
    // Angka Speed Besar
    canvas->setTextSize(4);
    canvas->setCursor(75, 70);
    canvas->printf("%03d", speed);

    // --- 2. KANAN ATAS: SEIN ---
    uint16_t leftColor = (steerState == 1 && blinkState) ? 0x07E0 : 0x0010;
    uint16_t rightColor = (steerState == 2 && blinkState) ? 0x07E0 : 0x0010;
    canvas->fillTriangle(220, 30, 240, 20, 240, 40, leftColor); // Sein Kiri
    canvas->fillTriangle(280, 30, 300, 20, 300, 40, rightColor); // Sein Kanan

    // --- 3. KANAN TENGAH: BATT & SIGNAL ---
    canvas->setTextSize(1);
    canvas->setCursor(220, 60); canvas->printf("BATT: %d%%", batt);
    canvas->setCursor(220, 80); canvas->printf("SIG : %d dBm", sig);

    // --- 4. KANAN BAWAH: BAR RPM ---
    canvas->setCursor(220, 120); canvas->print("RPM");
    canvas->drawRect(220, 135, 90, 10, 0x0010); // Border
    canvas->fillRect(221, 136, map(rpm, 0, 100, 0, 88), 8, 0x07E0); // Bar Hijau

    // Render ke layar
    gfx->draw16bitRGBBitmap(0, 0, canvas->getFramebuffer(), 320, 172);
}
