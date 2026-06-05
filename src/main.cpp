#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <math.h>

// PIN DEFINITION (Waveshare ESP32-C6)
#define TFT_BL 22
#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS   14
#define TFT_DC   15
#define TFT_RST  21

// Konfigurasi Layar 1.47" (PENTING: Offset 34, 0 agar tidak noise)
Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, GFX_NOT_DEFINED);
Arduino_GFX *gfx = new Arduino_ST7789(bus, TFT_RST, 3, true, 172, 320, 34, 0, 34, 0);
Arduino_Canvas *canvas = new Arduino_Canvas(320, 172, gfx);

void drawDashboard(int speed, int rpm, int batt) {
    canvas->fillScreen(0x0000); // Background Hitam

    // 1. Gambar Lingkaran Gauge
    int cx = 85, cy = 85, r = 70;
    canvas->drawCircle(cx, cy, r, 0x5AEB);

    // 2. Garis Skala Speedometer
    for (int i = 0; i <= 200; i += 20) {
        int angle = map(i, 0, 200, 140, 400);
        float rad = angle * M_PI / 180.0;
        canvas->drawLine(cx + (int)(cos(rad)*(r-8)), cy + (int)(sin(rad)*(r-8)), 
                         cx + (int)(cos(rad)*r), cy + (int)(sin(rad)*r), 0xFFFF);
    }

    // 3. Jarum Speedometer
    int angle = map(constrain(speed, 0, 200), 0, 200, 140, 400);
    float rad = angle * M_PI / 180.0;
    canvas->drawLine(cx, cy, cx + (int)(cos(rad)*55), cy + (int)(sin(rad)*55), 0xF800);
    canvas->fillCircle(cx, cy, 4, 0xF800);

    // 4. Bar RPM (Kanan)
    canvas->drawRect(190, 50, 100, 25, 0x5AEB);
    canvas->fillRect(192, 52, map(constrain(rpm, 0, 100), 0, 100, 0, 96), 21, 0x07E0);

    // 5. Teks KMH & Baterai
    canvas->setCursor(200, 100);
    canvas->setTextSize(2);
    canvas->printf("%03d KMH", speed);
    canvas->setCursor(220, 20);
    canvas->printf("%d%%", batt);

    // Update layar fisik
    gfx->draw16bitRGBBitmap(0, 0, canvas->getFramebuffer(), 320, 172);
}

void setup() {
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
    gfx->begin();
    canvas->begin();
}

void loop() {
    // Simulasi data
    drawDashboard(85, 60, 89);
    delay(50);
}
