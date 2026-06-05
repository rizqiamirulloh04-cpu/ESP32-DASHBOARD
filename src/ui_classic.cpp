#include "ui_classic.h"

extern Arduino_GFX *gfx; // Menunjuk ke variabel gfx di main.cpp
Arduino_Canvas *canvas;

void init_ui(Arduino_GFX *gfx_ptr) {
    canvas = new Arduino_Canvas(320, 172, gfx_ptr);
    canvas->begin();
}

void draw_ui_classic(int speed, int rpm, int batt, int sig, int steerState, bool blinkState) {
    canvas->fillScreen(0x0000); // Black

    // --- Tulis semua logika menggambar kamu di sini ---
    // Contoh:
    canvas->setTextColor(0xFFFF);
    canvas->setCursor(10, 10);
    canvas->printf("SPEED: %d", speed);

    // Transfer ke layar fisik
    gfx->draw16bitRGBBitmap(0, 0, canvas->getFramebuffer(), 320, 172);
}
