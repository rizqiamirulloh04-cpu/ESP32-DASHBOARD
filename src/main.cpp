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
#define STEER_PIN 1
#define THROTTLE_PIN 2

Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, GFX_NOT_DEFINED);
Arduino_GFX *gfx = new Arduino_ST7789(bus, TFT_RST, 3, true, 172, 320, 34, 0, 34, 0);
Arduino_Canvas *canvas = new Arduino_Canvas(320, 172, gfx);

// Fungsi ini harus bernama 'drawDashboard' agar sesuai dengan pemanggilan di loop
void drawDashboard(int speed, int rpm, int batt) {
    canvas->fillScreen(0x0000); 

    int cx = 85, cy = 85, r = 70;
    canvas->drawCircle(cx, cy, r, 0x5AEB);

    // Loop untuk garis dan angka
    for (int v = 0; v <= 120; v += 10) {
        int angle = map(v, 0, 120, 140, 400);
        float rad = angle * M_PI / 180.0;

        if (v % 20 == 0) {
            // Garis PUTIH (panjang)
            canvas->drawLine(cx + (int)(cos(rad) * (r - 12)), cy + (int)(sin(rad) * (r - 12)), 
                             cx + (int)(cos(rad) * r), cy + (int)(sin(rad) * r), 0xFFFF);
            // Angka
            int tx = cx + (int)(cos(rad) * (r - 28));
            int ty = cy + (int)(sin(rad) * (r - 28));
            canvas->setCursor(tx - 6, ty - 4);
            canvas->setTextSize(1);
            canvas->print(v);
        } else {
            // Garis UNGU CERAH (pendek) - 0xF81F
            canvas->drawLine(cx + (int)(cos(rad) * (r - 6)), cy + (int)(sin(rad) * (r - 6)), 
                             cx + (int)(cos(rad) * r), cy + (int)(sin(rad) * r), 0xF81F);
        }
    }

    // Jarum
    int nAngle = map(constrain(speed, 0, 120), 0, 120, 140, 400);
    float nRad = nAngle * M_PI / 180.0;
    canvas->drawLine(cx, cy, cx + (int)(cos(nRad)*55), cy + (int)(sin(nRad)*55), 0xF800);
    canvas->fillCircle(cx, cy, 4, 0xF800);

    // Bar RPM & Info
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
    int steerPulse = pulseIn(STEER_PIN, HIGH, 20000);
    int speed = map(steerPulse, 1000, 2000, 0, 120);
    int throtPulse = pulseIn(THROTTLE_PIN, HIGH, 20000);
    int rpm = map(throtPulse, 1000, 2000, 0, 100);
    
    // Panggil fungsi yang benar
    drawDashboard(speed, rpm, 89);
    delay(30);
}
