#include "ui_classic.h"
#include <math.h>

extern Arduino_GFX *gfx;
Arduino_Canvas *canvas;

void init_ui(Arduino_GFX *gfx_ptr) {
    canvas = new Arduino_Canvas(320, 172, gfx_ptr);
    canvas->begin();
}

void draw_ui_classic(int speed, int rpm, int batt, int sig, int steerState, bool blinkState) {
    if (!canvas) return;
    canvas->fillScreen(0x0000); 

    // --- BAGIAN KIRI: SPEEDOMETER ANALOG ---
    int centerX = 80; 
    int centerY = 86;
    int radius = 70;
    canvas->drawCircle(centerX, centerY, radius, 0x5AEB); 
    
    // Skala & Angka
    for (int i = 0; i <= 200; i += 40) {
        int angle = map(i, 0, 200, 140, 400);
        float rad = angle * M_PI / 180.0;
        int x1 = centerX + (int)(cos(rad) * (radius - 10));
        int y1 = centerY + (int)(sin(rad) * (radius - 10));
        int x2 = centerX + (int)(cos(rad) * radius);
        int y2 = centerY + (int)(sin(rad) * radius);
        canvas->drawLine(x1, y1, x2, y2, 0xFFFF);
        int tx = centerX + (int)(cos(rad) * (radius - 22));
        int ty = centerY + (int)(sin(rad) * (radius - 22));
        canvas->setCursor(tx - 5, ty - 3);
        canvas->print(i);
    }
    // Jarum
    int angle = map(constrain(speed, 0, 200), 0, 200, 140, 400);
    float rad = angle * M_PI / 180.0;
    canvas->drawLine(centerX, centerY, centerX + (int)(cos(rad)*55), centerY + (int)(sin(rad)*55), 0xF800);
    canvas->fillCircle(centerX, centerY, 5, 0xF800);

    // --- BAGIAN KANAN: DASHBOARD INFORMASI ---
    // 1. Indikator Kiri/Kanan (Blink)
    if(blinkState) {
        canvas->fillCircle(200, 20, 8, 0x07E0); // Sen Kiri
        canvas->fillCircle(280, 20, 8, 0x07E0); // Sen Kanan
    }
    // 2. Baterai (Tengah Atas)
    canvas->drawRect(220, 10, 40, 20, 0xFFFF);
    canvas->fillRect(222, 12, map(batt, 0, 100, 0, 36), 16, 0x07E0);
    
    // 3. Kotak Tengah: Bar RPM
    canvas->drawRect(180, 50, 120, 40, 0x5AEB);
    canvas->fillRect(182, 52, map(constrain(rpm, 0, 100), 0, 100, 0, 116), 36, 0xF800);
    
    // 4. Kotak Bawah: KMH Digital
    canvas->setCursor(210, 110);
    canvas->setTextSize(3);
    canvas->setTextColor(0x07FF); // Warna Cyan
    canvas->printf("%03d", speed);
    canvas->setCursor(270, 120);
    canvas->setTextSize(1);
    canvas->print("KMH");

    gfx->draw16bitRGBBitmap(0, 0, canvas->getFramebuffer(), 320, 172);
}
