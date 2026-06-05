#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <math.h>

// --- PIN CONFIG (WAJIB SESUAI ESP32-C6) ---
#define TFT_BL 22
#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS   14
#define TFT_DC   15
#define TFT_RST  21
#define STEER_PIN    1
#define THROTTLE_PIN 2

// --- WARNA ---
#define BLACK  0x0000
#define WHITE  0xFFFF
#define CYAN   0x07FF
#define YELLOW 0xFFE0
#define GRAY   0x5AEB

// --- INISIALISASI DISPLAY ---
Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, GFX_NOT_DEFINED);
Arduino_GFX *gfx = new Arduino_ST7789(bus, TFT_RST, 1, true, 172, 320, 34, 0, 34, 0);
Arduino_Canvas *canvas = new Arduino_Canvas(320, 172, gfx);

// --- VARIABEL ---
float filteredThrottle = 1500.0;
float filteredSteer = 1500.0;
const float SMOOTH_THROTTLE = 0.15;
const float SMOOTH_STEER = 0.08;

void drawSteeringIndicator(int x, int y, float steerVal) {
    uint16_t steerColor = (steerVal < 1350 || steerVal > 1650) ? YELLOW : CYAN;
    canvas->drawCircle(x, y, 15, GRAY);
    float angle = map(constrain((int)steerVal, 1000, 2000), 1000, 2000, -45, 45) * M_PI / 180.0;
    int x1 = x + (int)(12 * sin(angle));
    int y1 = y - (int)(12 * cos(angle));
    int x2 = x - (int)(12 * sin(angle));
    int y2 = y + (int)(12 * cos(angle));
    canvas->drawLine(x1, y1, x2, y2, steerColor);
}

void drawGauge(int cx, int cy, int radius, int speed) {
    canvas->drawCircle(cx, cy, radius, GRAY);
    for (int i = 0; i <= 240; i += 20) {
        float angle = map(i, 0, 240, 140, 400) * M_PI / 180.0;
        canvas->drawLine(cx + (radius-8)*cos(angle), cy + (radius-8)*sin(angle), 
                         cx + radius*cos(angle), cy + radius*sin(angle), WHITE);
    }
    float angle = map(constrain(speed, 0, 240), 0, 240, 140, 400) * M_PI / 180.0;
    canvas->drawLine(cx, cy, cx + (radius-15)*cos(angle), cy + (radius-15)*sin(angle), YELLOW);
    canvas->fillCircle(cx, cy, 5, YELLOW);
}

void setup() {
    // BACKLIGHT SETUP (MENGATASI LAYAR GELAP)
    ledcAttach(TFT_BL, 5000, 8);
    ledcWrite(TFT_BL, 200); 

    gfx->begin();
    gfx->setRotation(1);
    canvas->begin();
    
    pinMode(STEER_PIN, INPUT);
    pinMode(THROTTLE_PIN, INPUT);
}

void loop() {
    int rawThrottle = pulseIn(THROTTLE_PIN, HIGH, 20000);
    int rawSteer = pulseIn(STEER_PIN, HIGH, 20000);
    if(rawThrottle == 0) rawThrottle = 1500;
    if(rawSteer == 0) rawSteer = 1500;

    filteredThrottle = (filteredThrottle * (1.0 - SMOOTH_THROTTLE)) + (rawThrottle * SMOOTH_THROTTLE);
    filteredSteer = (filteredSteer * (1.0 - SMOOTH_STEER)) + (rawSteer * SMOOTH_STEER);

    int speed = map(constrain((int)filteredThrottle, 1050, 1950), 1050, 1950, 0, 240);
    if(speed < 10) speed = 0;

    canvas->fillScreen(BLACK);
    drawGauge(160, 85, 75, speed);
    drawSteeringIndicator(260, 85, filteredSteer);
    
    canvas->setCursor(140, 140);
    canvas->setTextColor(WHITE);
    canvas->printf("%03d KM/H", speed);

    gfx->draw16bitRGBBitmap(0, 0, canvas->getFramebuffer(), 320, 172);
}
