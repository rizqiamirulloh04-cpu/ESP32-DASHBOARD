#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <math.h>

#define TFT_BL 22
#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS   14
#define TFT_DC   15
#define TFT_RST  21
#define STEER_PIN    1
#define THROTTLE_PIN 2

Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, GFX_NOT_DEFINED);
Arduino_GFX *gfx = new Arduino_ST7789(bus, TFT_RST, 1, true, 172, 320, 34, 0, 34, 0);
Arduino_Canvas *canvas = new Arduino_Canvas(320, 172, gfx);

float filteredThrottle = 1050.0;
float filteredSteer = 1500.0;

// Fungsi untuk menggambar panah neon
void drawNeonArrow(int x, int y, bool right, uint16_t color) {
    int dir = right ? 1 : -1;
    for(int i = 0; i < 3; i++) {
        int ox = x + (i * 12 * dir);
        canvas->fillTriangle(ox, y, ox + (10 * dir), y - 6, ox + (10 * dir), y + 6, color);
    }
}

void drawBackground() {
    // Membuat kesan gradasi biru gelap ke hitam
    for(int r = 160; r > 0; r -= 20) {
        uint16_t color = (r > 80) ? 0x0008 : 0x0000;
        canvas->fillCircle(160, 85, r, color);
    }
}

void drawGauge(int speed) {
    int cx = 160, cy = 85, r = 75;
    // Bingkai Luar
    canvas->drawCircle(cx, cy, r, 0x5AEB);
    canvas->drawCircle(cx, cy, r - 2, 0x001F);
    
    // Skala
    for (int i = 0; i <= 240; i += 20) {
        float angle = map(i, 0, 240, 135, 405) * M_PI / 180.0;
        canvas->drawLine(cx + (r-10)*cos(angle), cy + (r-10)*sin(angle), 
                         cx + r*cos(angle), cy + r*sin(angle), WHITE);
    }

    // Jarum
    float angle = map(constrain(speed, 0, 240), 0, 240, 135, 405) * M_PI / 180.0;
    canvas->drawLine(cx, cy, cx + (r-15)*cos(angle), cy + (r-15)*sin(angle), 0xFFE0);
    canvas->fillCircle(cx, cy, 6, 0xFFE0);
}

void setup() {
    ledcAttach(TFT_BL, 5000, 8);
    ledcWrite(TFT_BL, 200);
    gfx->begin();
    gfx->setRotation(1);
    canvas->begin();
}

void loop() {
    int rawThrottle = pulseIn(THROTTLE_PIN, HIGH, 20000);
    int rawSteer = pulseIn(STEER_PIN, HIGH, 20000);
    
    filteredThrottle = (filteredThrottle * 0.9) + ((rawThrottle > 0 ? rawThrottle : 1050) * 0.1);
    filteredSteer = (filteredSteer * 0.95) + ((rawSteer > 0 ? rawSteer : 1500) * 0.05);

    int speed = map(constrain((int)filteredThrottle, 1050, 1950), 1050, 1950, 0, 240);

    canvas->fillScreen(0x0000);
    drawBackground();
    drawGauge(speed);
    
    // Indikator Panah Samping (Neon)
    drawNeonArrow(250, 50, true, (filteredSteer > 1650) ? 0xF800 : 0x001F); // Merah/Biru
    drawNeonArrow(250, 120, true, (filteredSteer < 1350) ? 0x07E0 : 0x001F); // Hijau/Biru

    canvas->setCursor(140, 150);
    canvas->setTextColor(WHITE);
    canvas->printf("%03d KM/H", speed);

    gfx->draw16bitRGBBitmap(0, 0, canvas->getFramebuffer(), 320, 172);
}
