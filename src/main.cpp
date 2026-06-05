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
#define STEER_PIN 1
#define THROTTLE_PIN 2

// Konfigurasi Layar (Offset 34, 0 untuk 1.47")
Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, GFX_NOT_DEFINED);
Arduino_GFX *gfx = new Arduino_ST7789(bus, TFT_RST, 3, true, 172, 320, 34, 0, 34, 0);
Arduino_Canvas *canvas = new Arduino_Canvas(320, 172, gfx);

void drawDashboard(int speed, int rpm) {
    canvas->fillScreen(0x0000); // Background hitam bersih

    int cx = 85, cy = 85, r = 70;
    canvas->drawCircle(cx, cy, r, 0x5AEB);

    // Loop untuk garis skala (interval 10)
    for (int v = 0; v <= 120; v += 10) {
        int angle = map(v, 0, 120, 140, 400);
        float rad = angle * M_PI / 180.0;

        bool isMajor = (v % 20 == 0);
        
        // Garis tebal putih untuk angka, garis KUNING TERANG untuk garis tipis
        // Kita buat garis tipis lebih panjang sedikit (9) agar terlihat jelas
        int len = isMajor ? 12 : 9; 
        uint16_t color = isMajor ? 0xFFFF : 0xFFE0; // Putih : Kuning Terang

        canvas->drawLine(cx + (int)(cos(rad) * (r - len)), cy + (int)(sin(rad) * (r - len)), 
                         cx + (int)(cos(rad) * r), cy + (int)(sin(rad) * r), color);

        if (isMajor) {
            int tx = cx + (int)(cos(rad) * (r - 28));
            int ty = cy + (int)(sin(rad) * (r - 28));
            canvas->setCursor(tx - 6, ty - 4);
            canvas->setTextSize(1);
            canvas->print(v);
        }
    }

    // Jarum Speedometer
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
    // Membaca input
    int steerPulse = pulseIn(STEER_PIN, HIGH, 20000);
    int speed = map(constrain(steerPulse, 1000, 2000), 1000, 2000, 0, 120);
    
    int throtPulse = pulseIn(THROTTLE_PIN, HIGH, 20000);
    int rpm = map(constrain(throtPulse, 1000, 2000), 1000, 2000, 0, 100);
    
    drawDashboard(speed, rpm);
    delay(30);
}
