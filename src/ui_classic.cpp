#include "ui_classic.h"
#include <math.h>

extern Arduino_GFX *gfx;
Arduino_Canvas *canvas;

// Fungsi untuk menggambar busur (Arc) yang halus
void drawArc(int cx, int cy, int r, int startDeg, int endDeg, uint16_t color) {
    for (int i = startDeg; i <= endDeg; i++) {
        float rad = i * M_PI / 180.0;
        int x = cx + cos(rad) * r;
        int y = cy + sin(rad) * r;
        canvas->drawPixel(x, y, color);
    }
}

void draw_ui_classic(int speed, int rpm, int batt, int sig, int steerState, bool blinkState) {
    canvas->fillScreen(0x0000); // Background Hitam

    // 1. Gambar Lingkaran Utama
    canvas->drawCircle(160, 85, 70, 0x5AEB); // Lingkaran luar
    
    // 2. Gambar Speedometer Arc (Biru)
    drawArc(160, 85, 65, 140, 400, 0x001F); 
    
    // 3. Tampilkan Angka Speed di Tengah
    canvas->setTextSize(4);
    canvas->setTextColor(0xFFFF);
    canvas->setCursor(120, 70);
    canvas->printf("%03d", speed);

    // 4. Bar Indikator (Kanan - Contoh Desain)
    canvas->drawRect(250, 40, 40, 80, 0x5AEB);
    int barHeight = map(rpm, 0, 100, 0, 78);
    canvas->fillRect(251, 119 - barHeight, 38, barHeight, 0xF800); // Bar merah

    // Update layar
    gfx->draw16bitRGBBitmap(0, 0, canvas->getFramebuffer(), 320, 172);
}
