#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// ======================================================
// COLOR
// ======================================================

#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
#define CYAN    0x07FF

// ======================================================
// WAVESHARE ESP32-C6 LCD 1.47
// ======================================================

#define TFT_BL    15

#define TFT_DC    8
#define TFT_CS    14
#define TFT_RST   21

// ======================================================
// SPI DISPLAY
// ======================================================

Arduino_DataBus *bus = new Arduino_HWSPI(
    TFT_DC,
    TFT_CS
);

Arduino_GFX *gfx = new Arduino_ST7789(
    bus,
    TFT_RST,
    1,      // rotation
    true,   // IPS
    172,    // width
    320,    // height
    34,     // col offset 1
    0,      // row offset 1
    34,     // col offset 2
    0       // row offset 2
);

// ======================================================
// SETUP
// ======================================================

void setup()
{
    Serial.begin(115200);

    // =========================
    // BACKLIGHT
    // =========================

    pinMode(TFT_BL, OUTPUT);

    // COBA HIGH DULU
    digitalWrite(TFT_BL, HIGH);

    // =========================
    // LCD RESET
    // =========================

    pinMode(TFT_RST, OUTPUT);

    digitalWrite(TFT_RST, HIGH);
    delay(100);

    digitalWrite(TFT_RST, LOW);
    delay(100);

    digitalWrite(TFT_RST, HIGH);
    delay(200);

    // =========================
    // DISPLAY INIT
    // =========================

    gfx->begin();

    gfx->invertDisplay(true);

    gfx->setRotation(1);

    // =========================
    // TEST SCREEN
    // =========================

    gfx->fillScreen(BLACK);

    gfx->drawRect(
        0,
        0,
        320,
        172,
        CYAN
    );

    // =========================
    // TEXT
    // =========================

    gfx->setTextColor(WHITE);

    gfx->setTextSize(2);

    gfx->setCursor(20, 40);
    gfx->println("DISPLAY OK");

    gfx->setCursor(20, 80);
    gfx->println("ESP32-C6");

    gfx->setCursor(20, 120);
    gfx->println("WAVESHARE 1.47");

    // =========================
    // COLOR TEST
    // =========================

    gfx->fillRect(250, 20, 40, 40, RED);

    gfx->fillRect(250, 70, 40, 40, GREEN);

    gfx->fillRect(250, 120, 40, 40, BLUE);

    Serial.println("LCD TEST DONE");
}

// ======================================================
// LOOP
// ======================================================

void loop()
{
}