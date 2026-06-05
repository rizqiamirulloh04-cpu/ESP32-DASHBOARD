#include "ui_classic.h"
#include <math.h>

extern Arduino_GFX *gfx;
Arduino_Canvas *canvas;

void drawArc(int cx, int cy, int r, int startDeg, int endDeg, uint16_t color) {
    for (int i = startDeg; i <= endDeg; i++) {
        float rad = i * M_PI / 180.0;
        int x = cx + cos(rad) * r;
        int y = cy + sin(rad) * r;
        canvas->drawPixel(x, y, color);
    }
}

void draw_ui_classic(int speed, int rpm, int batt, int sig, int steerState, bool blinkState) {
    canvas->fillScreen(0x0000); 

    // PUSAT LINGKARAN BARU: cx = 85 (lebih ke kiri), cy = 85 (tetap di tengah vertikal)
    int centerX = 85;
    int centerY = 85;

    // 1. Gambar Lingkaran Utama
    canvas->drawCircle(centerX, centerY, 70, 0x5AEB); 
    
    // 2. Gambar Speedometer Arc (Biru)
    drawArc(centerX, centerY, 65, 140, 400, 0x001F); 
    
    // 3. Tampilkan Angka Speed di Tengah Lingkaran (kini di kiri)
    canvas->setTextSize(4);
    canvas->setTextColor(0xFFFF);
    canvas->setCursor(centerX - 40, centerY - 20); // Disesuaikan dengan center baru
    canvas->printf("%03d", speed);

    // 4. Bar Indikator (Sekarang bisa diletakkan di sisi KANAN layar yang kosong)
    canvas->drawRect(200, 40, 40, 80, 0x5AEB);
    int barHeight = map(rpm, 0, 100, 0, 78);
    canvas->fillRect(201, 119 - barHeight, 38, barHeight, 0xF800); 

    gfx->draw16bitRGBBitmap(0, 0, canvas->getFramebuffer(), 320, 172);
}
