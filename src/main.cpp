#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <math.h>

// PIN DEFINITION
#define TFT_BL 22
#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS   14
#define TFT_DC   15
#define TFT_RST  21
#define RX_PIN   1 

// DRIVER & CANVAS (Konfigurasi Final 1.47" yang sudah terbukti)
Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, GFX_NOT_DEFINED);
Arduino_GFX *gfx = new Arduino_ST7789(bus, TFT_RST, 3, true, 172, 320, 34, 0, 34, 0);
Arduino_Canvas *canvas = new Arduino_Canvas(320, 172, gfx);

// FUNGSI GRAFIS (Diambil dari ui_classic.cpp Anda)
void drawArrow(int x, int y, bool isLeft, uint16_t color) {
    if (isLeft) {
        canvas->fillTriangle(x, y, x + 10, y - 8, x + 10, y + 8, color);
        canvas->fillRect(x + 10, y - 4, 5, 8, color);
    } else {
        canvas->fillTriangle(x + 15, y, x + 5, y - 8, x + 5, y + 8, color);
        canvas->fillRect(x, y - 4, 5, 8, color);
    }
}

void draw_ui(int speed, int rpm, int batt, bool blinkState) {
    canvas->fillScreen(0x0000); 

    // GAUGE SPEEDOMETER
    int centerX = 85, centerY = 85, radius = 70;
    canvas->drawCircle(centerX, centerY, radius, 0x5AEB); 
    for (int i = 0; i <= 200; i += 10) { 
        int angle = map(i, 0, 200, 140, 400);
        float rad = angle * M_PI / 180.0;
        int innerR = (i % 40 == 0) ? (radius - 12) : (radius - 6);
        canvas->drawLine(centerX + (int)(cos(rad)*innerR), centerY + (int)(sin(rad)*sin(rad) + cos(rad)*innerR), 
                         centerX + (int)(cos(rad)*radius), centerY + (int)(sin(rad)*radius), 0xFFFF);
    }

    // JARUM SPEEDO
    int angle = map(constrain(speed, 0, 200), 0, 200, 140, 400);
    float rad = angle * M_PI / 180.0;
    canvas->drawLine(centerX, centerY, centerX + (int)(cos(rad)*55), centerY + (int)(sin(rad)*55), 0xF800);
    canvas->fillCircle(centerX, centerY, 5, 0xF800);

    // RPM BAR & DATA
    canvas->drawRect(180, 50, 120, 40, 0x5AEB);
    canvas->fillRect(182, 52, map(constrain(rpm, 0, 100), 0, 100, 0, 116), 36, 0xF800);
    
    // ANGKA KMH
    canvas->setCursor(210, 110);
    canvas->setTextSize(3);
    canvas->setTextColor(0x07FF);
    canvas->printf("%03d", speed);
    canvas->setCursor(270, 120);
    canvas->setTextSize(1);
    canvas->print("KMH");

    if(blinkState) drawArrow(190, 20, true, 0x07E0);
    
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
    int speed = map(pulse, 1000, 2000, 0, 200); // Sesuaikan range 0-200 kmh
    
    draw_ui(speed, speed, 80, true); 
    delay(50);
}
