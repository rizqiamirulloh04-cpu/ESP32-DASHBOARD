#include "ui_classic.h"
#include <math.h>

extern Arduino_GFX *gfx;
Arduino_Canvas *canvas;

// Fungsi inisialisasi WAJIB ADA agar main.cpp tidak error
void init_ui(Arduino_GFX *gfx_ptr) {
    canvas = new Arduino_Canvas(320, 172, gfx_ptr);
    canvas->begin();
}

void drawArc(int cx, int cy, int r, int startDeg, int endDeg, uint16_t color) {
    for (int i = startDeg; i <= endDeg; i++) {
        float rad = i * M_PI / 180.0;
        int x = cx + (int)(cos(rad) * r);
        int y = cy + (int)(sin(rad) * r);
        // Memastikan tidak menggambar di luar batas layar
        if (x >= 0 && x < 320 && y >= 0 && y < 172) {
            canvas->drawPixel(x, y, color);
        }
    }
}

void draw_ui_classic(int speed, int rpm, int batt, int sig, int steerState, bool blinkState) {
    if (!canvas) return;
    canvas->fillScreen(0x0000); 

    int centerX = 85;
    int centerY = 85;

    // 1. Gambar Lingkaran Utama
    canvas->drawCircle(centerX, centerY, 70, 0x5AEB); 
    
    // 2. Gambar Speedometer Arc (Biru)
    drawArc(centerX, centerY, 65, 140, 400, 0x001F); 

    // 3. JARUM MERAH MENYALA (Pengganti Angka)
    // Mapping speed (0-120) ke sudut (140-400 derajat)
    int angle = map(constrain(speed, 0, 120), 0, 120, 140, 400);
    float rad = angle * M_PI / 180.0;
    
    // Koordinat ujung jarum
    int x2 = centerX + (int)(cos(rad) * 55);
    int y2 = centerY + (int)(sin(rad) * 55);
    
    // Menggambar jarum dengan efek tebal (3 baris garis untuk efek menyala)
    canvas->drawLine(centerX, centerY, x2, y2, 0xF800); // Merah utama
    canvas->drawLine(centerX+1, centerY+1, x2+1, y2+1, 0xF800); // Shadow/Glow
    canvas->fillCircle(centerX, centerY, 5, 0xF800); // Pusat jarum

    // 4. Bar Indikator RPM (Kanan)
    canvas->drawRect(200, 40, 40, 80, 0x5AEB);
    int barHeight = map(constrain(rpm, 0, 100), 0, 100, 0, 78);
    canvas->fillRect(201, 119 - barHeight, 38, barHeight, 0xF800); 

    gfx->draw16bitRGBBitmap(0, 0, canvas->getFramebuffer(), 320, 172);
}
