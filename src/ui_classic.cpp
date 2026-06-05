#include "ui_classic.h"
#include <math.h>

extern Arduino_GFX *gfx;
Arduino_Canvas *canvas;

void init_ui(Arduino_GFX *gfx_ptr) {
    canvas = new Arduino_Canvas(320, 172, gfx_ptr);
    canvas->begin();
}

void drawArc(int cx, int cy, int r, int startDeg, int endDeg, uint16_t color) {
    for (int i = startDeg; i <= endDeg; i++) {
        float rad = i * M_PI / 180.0;
        int x = cx + (int)(cos(rad) * r);
        int y = cy + (int)(sin(rad) * r);
        if (x >= 0 && x < 320 && y >= 0 && y < 172) {
            canvas->drawPixel(x, y, color);
        }
    }
}

void draw_ui_classic(int speed, int rpm, int batt, int sig, int steerState, bool blinkState) {
    if (!canvas) return;
    canvas->fillScreen(0x0000); 

    int centerX = 235; // Lingkaran di kanan
    int centerY = 85;
    int radius = 70;

    // 1. Lingkaran Utama
    canvas->drawCircle(centerX, centerY, radius, 0x5AEB); 

    // 2. Skala Garis & Angka
    canvas->setTextColor(0xFFFF);
    canvas->setTextSize(1);
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

    // 3. Jarum Merah
    int angle = map(constrain(speed, 0, 200), 0, 200, 140, 400);
    float rad = angle * M_PI / 180.0;
    int x2 = centerX + (int)(cos(rad) * 55);
    int y2 = centerY + (int)(sin(rad) * 55);
    
    canvas->drawLine(centerX, centerY, x2, y2, 0xF800);
    canvas->drawLine(centerX+1, centerY+1, x2+1, y2+1, 0xF800);
    canvas->fillCircle(centerX, centerY, 5, 0xF800);

    // 4. Bar Indikator RPM (Kiri)
    canvas->drawRect(40, 40, 40, 80, 0x5AEB);
    int barHeight = map(constrain(rpm, 0, 100), 0, 100, 0, 78);
    canvas->fillRect(41, 119 - barHeight, 38, barHeight, 0xF800); 

    // Render ke layar fisik
    gfx->draw16bitRGBBitmap(0, 0, canvas->getFramebuffer(), 320, 172);
}
