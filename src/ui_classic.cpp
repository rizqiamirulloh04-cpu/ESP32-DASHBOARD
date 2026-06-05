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

    int centerX = 85; 
    int centerY = 85;
    int radius = 70;

    // 1. Lingkaran Utama
    canvas->drawCircle(centerX, centerY, radius, 0x5AEB); 

    // 2. Skala Garis & Angka
    for (int i = 0; i <= 200; i += 10) { 
        int angle = map(i, 0, 200, 140, 400);
        float rad = angle * M_PI / 180.0;

        // Tentukan panjang garis
        int innerR = (i % 40 == 0) ? (radius - 12) : (radius - 6);
        
        int x1 = centerX + (int)(cos(rad) * innerR);
        int y1 = centerY + (int)(sin(rad) * innerR);
        int x2 = centerX + (int)(cos(rad) * radius);
        int y2 = centerY + (int)(sin(rad) * radius);
        
        canvas->drawLine(x1, y1, x2, y2, 0xFFFF);

        // Hanya gambar angka jika i adalah kelipatan 40
        if (i % 40 == 0) {
            int tx = centerX + (int)(cos(rad) * (radius + 12));
            int ty = centerY + (int)(sin(rad) * (radius + 12));
            canvas->setCursor(tx - 5, ty - 4);
            canvas->print(i);
        }
    }

    // 3. Jarum Merah
    int angle = map(constrain(speed, 0, 200), 0, 200, 140, 400);
    float rad = angle * M_PI / 180.0;
    int x2 = centerX + (int)(cos(rad) * 55);
    int y2 = centerY + (int)(sin(rad) * 55);
    
    canvas->drawLine(centerX, centerY, x2, y2, 0xF800);
    canvas->drawLine(centerX+1, centerY+1, x2+1, y2+1, 0xF800);
    canvas->fillCircle(centerX, centerY, 5, 0xF800);

    // KOTAK KANAN SUDAH DIHAPUS

    // Render ke layar fisik
    gfx->draw16bitRGBBitmap(0, 0, canvas->getFramebuffer(), 320, 172);
}
