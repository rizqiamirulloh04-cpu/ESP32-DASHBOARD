#include "ui_classic.h"
#include <math.h>

extern Arduino_GFX *gfx;
Arduino_Canvas *canvas;

void init_ui(Arduino_GFX *gfx_ptr) {
    canvas = new Arduino_Canvas(320, 172, gfx_ptr);
    canvas->begin();
}

void drawArrow(int x, int y, bool isLeft, uint16_t color) {
    if (isLeft) {
        canvas->fillTriangle(x, y, x + 10, y - 8, x + 10, y + 8, color);
        canvas->fillRect(x + 10, y - 4, 5, 8, color);
    } else {
        canvas->fillTriangle(x + 15, y, x + 5, y - 8, x + 5, y + 8, color);
        canvas->fillRect(x, y - 4, 5, 8, color);
    }
}

void draw_ui_classic(int speed, int rpm, int batt, int sig, int steerState, bool blinkState) {
    if (!canvas) return;
    canvas->fillScreen(0x0000); 

    int centerX = 85; 
    int centerY = 85;
    int radius = 70;

    canvas->drawCircle(centerX, centerY, radius, 0x5AEB); 

    for (int i = 0; i <= 200; i += 10) { 
        int angle = map(i, 0, 200, 140, 400);
        float rad = angle * M_PI / 180.0;
        int innerR = (i % 40 == 0) ? (radius - 12) : (radius - 6);
        canvas->drawLine(centerX + (int)(cos(rad)*innerR), centerY + (int)(sin(rad)*innerR), 
                         centerX + (int)(cos(rad)*radius), centerY + (int)(sin(rad)*radius), 0xFFFF);
        if (i % 40 == 0) {
            int tx = centerX + (int)(cos(rad) * (radius + 12));
            int ty = centerY + (int)(sin(rad) * (radius + 12));
            canvas->setCursor(tx - 5, ty - 4);
            canvas->print(i);
        }
    }

    int angle = map(constrain(speed, 0, 200), 0, 200, 140, 400);
    float rad = angle * M_PI / 180.0;
    canvas->drawLine(centerX, centerY, centerX + (int)(cos(rad)*55), centerY + (int)(sin(rad)*55), 0xF800);
    canvas->drawLine(centerX+1, centerY+1, centerX + (int)(cos(rad)*55)+1, centerY + (int)(sin(rad)*55)+1, 0xF800);
    canvas->fillCircle(centerX, centerY, 5, 0xF800);

    if(blinkState) {
        drawArrow(190, 20, true, 0x07E0);  
        drawArrow(270, 20, false, 0x07E0); 
    }

    // Persentase Baterai - digeser ke 220 agar lebih ke kiri
    canvas->setCursor(220, 15); 
    canvas->setTextSize(2);
    canvas->setTextColor(0xFFFF); 
    canvas->printf("%d%%", constrain(batt, 0, 100));
    
    canvas->drawRect(180, 50, 120, 40, 0x5AEB);
    canvas->fillRect(182, 52, map(constrain(rpm, 0, 100), 0, 100, 0, 116), 36, 0xF800);
    
    canvas->setCursor(210, 110);
    canvas->setTextSize(3);
    canvas->setTextColor(0x07FF);
    canvas->printf("%03d", speed);
    canvas->setCursor(270, 120);
    canvas->setTextSize(1);
    canvas->print("KMH");

    gfx->draw16bitRGBBitmap(0, 0, canvas->getFramebuffer(), 320, 172);
}
