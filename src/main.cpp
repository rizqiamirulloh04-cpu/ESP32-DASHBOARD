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

#define STEER_PIN 1  // Pin input Steering
#define THROTTLE_PIN 2 // Pin input Throttle (Sesuaikan pin Anda)

Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, GFX_NOT_DEFINED);
Arduino_GFX *gfx = new Arduino_ST7789(bus, TFT_RST, 3, true, 172, 320, 34, 0, 34, 0);
Arduino_Canvas *canvas = new Arduino_Canvas(320, 172, gfx);

void drawUI(int speed, int rpm) {
    canvas->fillScreen(0x0000); 

    // --- SPEEDOMETER (Steering) ---
    int cx = 85, cy = 85, r = 70;
    canvas->drawCircle(cx, cy, r, 0x5AEB);
    // Skala (Garis 0-120)
    for (int v = 0; v <= 120; v += 10) {
        int angle = map(v, 0, 120, 140, 400);
        float rad = angle * M_PI / 180.0;
        int len = (v % 20 == 0) ? 12 : 6;
        canvas->drawLine(cx + (int)(cos(rad)*(r-len)), cy + (int)(sin(rad)*(r-len)), 
                         cx + (int)(cos(rad)*r), cy + (int)(sin(rad)*r), 0xFFFF);
    }
    // Jarum (dari Steering)
    int nAngle = map(constrain(speed, 0, 120), 0, 120, 140, 400);
    float nRad = nAngle * M_PI / 180.0;
    canvas->drawLine(cx, cy, cx + (int)(cos(nRad)*55), cy + (int)(sin(nRad)*55), 0xF800);

    // --- BAR RPM (Throttle) ---
    canvas->drawRect(190, 50, 100, 25, 0x5AEB);
    canvas->fillRect(192, 52, map(constrain(rpm, 0, 100), 0, 100, 0, 96), 21, 0x07E0);
    
    canvas->setCursor(210, 100);
    canvas->setTextSize(2);
    canvas->printf("%03d KMH", speed);

    gfx->draw16bitRGBBitmap(0, 0, canvas->getFramebuffer(), 320, 172);
}

void setup() {
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
    pinMode(STEER_PIN, INPUT_PULLUP);
    pinMode(THROTTLE_PIN, INPUT_PULLUP);
    gfx->begin();
    canvas->begin();
}

void loop() {
    // Membaca Steering untuk Speed
    int steerPulse = pulseIn(STEER_PIN, HIGH, 20000);
    int speed = map(steerPulse, 1000, 2000, 0, 120);
    
    // Membaca Throttle untuk RPM
    int throtPulse = pulseIn(THROTTLE_PIN, HIGH, 20000);
    int rpm = map(throtPulse, 1000, 2000, 0, 100);
    
    drawUI(speed, rpm);
    delay(30);
}
