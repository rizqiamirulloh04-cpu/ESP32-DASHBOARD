#include "ui_classic.h"
#include <math.h>

extern Arduino_GFX *gfx;
Arduino_Canvas *canvas;

// --- Variabel Animasi Smooth (LERP) ---
float animatedSpeed = 0;
const float SPEED_SMOOTH_FACTOR = 0.15; // Smooth factor 0.1 - 0.2 adalah yang terbaik

// --- Fungsi Helper (Migrasi) ---
void drawCustomOvalArc(int cx, int cy, int rx, int ry, int startDeg, int endDeg, uint16_t defaultColor, int thickness, bool drawTicks, bool isSpeedArc) {
    int totalAngles = endDeg - startDeg;
    for (int t = 0; t < thickness; t++) {
        int curRx = rx - t; int curRy = ry - t;
        int step = isSpeedArc ? 1 : 2; 
        for (int angle = startDeg; angle <= endDeg; angle += step) {
            float rad = (float)angle * M_PI / 180.0;
            int x = cx + (int)(cos(rad) * curRx);
            int y = cy + (int)(sin(rad) * curRy);
            uint16_t pixelColor = defaultColor;
            if (isSpeedArc && totalAngles > 0) {
                int currentPos = angle - startDeg; 
                if (currentPos >= totalAngles - 4) pixelColor = 0xFFFF; // WHITE
                else {
                    int redIntensity = map(currentPos, 0, totalAngles, 6, 31);
                    redIntensity = constrain(redIntensity, 6, 31);
                    pixelColor = (redIntensity << 11); 
                }
            }
            if (x >= 0 && x < 320 && y >= 0 && y < 172) canvas->drawPixel(x, y, pixelColor);
            if (drawTicks && t == 0 && (angle % 4 == 0)) {
                for (int tickLen = 1; tickLen <= 4; tickLen++) {
                    int tx = cx + (int)(cos(rad) * (rx + tickLen));
                    int ty = cy + (int)(sin(rad) * (ry + tickLen));
                    if (tx >= 0 && tx < 320 && ty >= 0 && ty < 172) canvas->drawPixel(tx, ty, 0x5AEB); // GRAY
                }
            }
        }
    }
}

void drawSignalIcon(int x, int y) {
    canvas->fillCircle(x + 10, y + 12, 2, 0x07E0);
    canvas->drawArc(x + 10, y + 12, 5, 4, 220, 320, 0x07E0);
    canvas->drawArc(x + 10, y + 12, 9, 8, 220, 320, 0x07E0);
}

void drawBatteryIcon(int x, int y) {
    canvas->drawRect(x, y + 3, 18, 10, 0x5AEB);
    canvas->fillRect(x + 18, y + 6, 2, 4, 0x5AEB);
    canvas->fillRect(x + 2, y + 5, 14, 6, 0x07E0);
}

// Tambahkan fungsi drawSteeringIcon & drawThermometerIcon milikmu di sini...

void init_ui(Arduino_GFX *gfx_ptr) {
    canvas = new Arduino_Canvas(320, 172, gfx_ptr);
    canvas->begin();
}

void draw_ui_classic(int speed, int rpm, int batt, int sig, int steerState, bool blinkState) {
    // 1. Logika Easing (Smooth Animation)
    animatedSpeed += (speed - animatedSpeed) * SPEED_SMOOTH_FACTOR;
    int smoothSpeed = (int)animatedSpeed;

    canvas->fillScreen(0x0000); // Hitam

    // 2. Rendering Speedometer menggunakan smoothSpeed
    int startAngle = 145;
    int endAngle = 395;
    int activeAngle = map(constrain(smoothSpeed, 0, 120), 0, 120, startAngle, endAngle);
    
    drawCustomOvalArc(160, 85, 86, 48, startAngle, endAngle, 0x0010, 3, true, false);
    if (smoothSpeed > 0) {
        drawCustomOvalArc(160, 85, 86, 48, startAngle, activeAngle, 0x0000, 3, false, true);
    }

    // 3. Rendering Indikator Samping
    drawSignalIcon(15, 4);
    drawBatteryIcon(250, 6);
    
    // 4. Rendering Angka Speed
    canvas->setTextSize(4);
    canvas->setTextColor(0xFFFF);
    canvas->setCursor(130, 70);
    canvas->printf("%03d", smoothSpeed);

    // Kirim ke layar
    gfx->draw16bitRGBBitmap(0, 0, canvas->getFramebuffer(), 320, 172);
}
