#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <math.h>

// PIN DEFINITION (Sesuaikan dengan setup Anda)
#define TFT_BL 22
#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS   14
#define TFT_DC   15
#define TFT_RST  21
#define RX_PIN   1

Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, GFX_NOT_DEFINED);
Arduino_GFX *gfx = new Arduino_ST7789(bus, TFT_RST, 3, true, 172, 320, 34, 0, 34, 0);
Arduino_Canvas *canvas = new Arduino_Canvas(320, 172, gfx);

void drawDashboard(int speed) {
    canvas->fillScreen(0x0000); 

    int cx = 85, cy = 85, r = 70;
    canvas->drawCircle(cx, cy, r, 0x5AEB);

    // Loop untuk garis dan angka (0, 10, 20, ..., 120)
    for (int v = 0; v <= 120; v += 10) {
        int angle = map(v, 0, 120, 140, 400);
        float rad = angle * M_PI / 180.0;
        
        // Garis tebal untuk kelipatan 20, garis tipis untuk kelipatan 10
        int len = (v % 20 == 0) ? 12 : 6; 
        
        int x1 = cx + (int)(cos(rad) * (r - len));
        int y1 = cy + (int)(sin(rad) * (r - len));
        int x2 = cx + (int)(cos(rad) * r);
        int y2 = cy + (int)(sin(rad) * r);
        
        canvas->drawLine(x1, y1, x2, y2, 0xFFFF);
        
        // Cetak angka hanya untuk kelipatan 20 agar rapi
        if (v % 20 == 0) {
            int tx = cx + (int)(cos(rad) * (r - 28));
            int ty = cy + (int)(sin(rad) * (r - 28));
            canvas->setCursor(tx - 6, ty - 4);
            canvas->setTextSize(1);
            canvas->print(v);
        }
    }

    // Jarum
    int needleAngle = map(constrain(speed, 0, 120), 0, 120, 140, 400);
    float nRad = needleAngle * M_PI / 180.0;
    canvas->drawLine(cx, cy, cx + (int)(cos(nRad)*55), cy + (int)(sin(nRad)*55), 0xF800);
    canvas->fillCircle(cx, cy, 4, 0xF800);

    gfx->draw16bitRGBBitmap(0, 0, canvas->getFramebuffer(), 320, 172);
}

void setup() {
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
    pinMode(RX_PIN, INPUT_PULLUP);
    gfx->begin();
    canvas->begin();
}

void loop() {
    int pulse = pulseIn(RX_PIN, HIGH, 20000);
    int speed = map(pulse, 1000, 2000, 0, 120);
    drawDashboard(speed);
    delay(30);
}
