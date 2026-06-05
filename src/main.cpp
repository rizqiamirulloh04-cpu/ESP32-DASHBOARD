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

// DRIVER & CANVAS (Konfigurasi 1.47" Offset 34, 0)
Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, GFX_NOT_DEFINED);
Arduino_GFX *gfx = new Arduino_ST7789(bus, TFT_RST, 3, true, 172, 320, 34, 0, 34, 0);
Arduino_Canvas *canvas = new Arduino_Canvas(320, 172, gfx);

void drawDashboard(int speed, int rpm, int batt) {
    canvas->fillScreen(0x0000); 

    // 1. Gauge Lingkaran
    int cx = 85, cy = 85, r = 70;
    canvas->drawCircle(cx, cy, r, 0x5AEB);

    // 2. Skala Garis & Angka 0-120
    int skala[] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 120};
    for (int i = 0; i < 12; i++) {
        // Sudut disesuaikan agar 0 di kiri bawah, 120 di kanan bawah
        int angle = map(skala[i], 0, 120, 140, 400); 
        float rad = angle * M_PI / 180.0;
        
        // Garis Skala
        canvas->drawLine(cx + (int)(cos(rad)*(r-10)), cy + (int)(sin(rad)*(r-10)), 
                         cx + (int)(cos(rad)*r), cy + (int)(sin(rad)*r), 0xFFFF);
        
        // Angka Skala (Diposisikan di luar garis)
        int tx = cx + (int)(cos(rad) * (r - 25));
        int ty = cy + (int)(sin(rad) * (r - 25));
        canvas->setCursor(tx - 6, ty - 4);
        canvas->setTextSize(1);
        canvas->print(skala[i]);
    }

    // 3. Jarum
    int needleAngle = map(constrain(speed, 0, 120), 0, 120, 140, 400);
    float nRad = needleAngle * M_PI / 180.0;
    canvas->drawLine(cx, cy, cx + (int)(cos(nRad)*55), cy + (int)(sin(nRad)*55), 0xF800);
    canvas->fillCircle(cx, cy, 4, 0xF800);

    // 4. Bar RPM & Info
    canvas->drawRect(190, 50, 100, 25, 0x5AEB);
    canvas->fillRect(192, 52, map(constrain(rpm, 0, 100), 0, 100, 0, 96), 21, 0x07E0);
    canvas->setCursor(210, 100);
    canvas->setTextSize(2);
    canvas->printf("%03d KMH", speed);
    canvas->setCursor(240, 20);
    canvas->printf("%d%%", batt);

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
    drawDashboard(speed, 50, 89);
    delay(30);
}
