#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// ==========================
// COLOR
// ==========================

#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
#define CYAN    0x07FF

// ==========================
// WAVESHARE ESP32-C6 LCD
// ==========================

#define TFT_BL   15

#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_DC   8
#define TFT_RST  9
#define TFT_CS   14

// ==========================
// SPI BUS
// ==========================

Arduino_DataBus *bus = new Arduino_ESP32SPI(
    TFT_DC,
    TFT_CS,
    TFT_SCLK,
    TFT_MOSI,
    GFX_NOT_DEFINED
);

// ==========================
// DISPLAY
// ==========================

Arduino_GFX *gfx = new Arduino_ST7789(
    bus,
    TFT_RST,
    1,      // rotation
    true,   // IPS
    172,    // width
    320,    // height
    34,     // col offset
    0,      // row offset
    35,     // col offset 2
    0       // row offset 2
);

// ==========================

void setup()
{
    Serial.begin(115200);

    // BACKLIGHT
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    // DISPLAY INIT
    gfx->begin(40000000);

    gfx->invertDisplay(false);

    // ======================
    // COLOR TEST
    // ======================

    gfx->fillScreen(BLACK);
    delay(500);

    gfx->fillScreen(RED);
    delay(1000);

    gfx->fillScreen(GREEN);
    delay(1000);

    gfx->fillScreen(BLUE);
    delay(1000);

    gfx->fillScreen(BLACK);

    // ======================
    // TEST UI
    // ======================

    gfx->drawRect(0, 0, 320, 172, WHITE);

    // TOP LINE
    gfx->drawFastHLine(70, 20, 180, CYAN);

    // TRIANGLE
    gfx->fillTriangle(
        160, 8,
        150, 20,
        170, 20,
        GREEN
    );

    // LEFT BAR
    gfx->fillRoundRect(
        40,
        40,
        8,
        100,
        3,
        RED
    );

    // RIGHT BAR
    gfx->fillRoundRect(
        272,
        40,
        8,
        100,
        3,
        CYAN
    );

    // SPEED
    gfx->setTextColor(WHITE);
    gfx->setTextSize(5);

    gfx->setCursor(90, 80);
    gfx->print("000");

    // KM/H
    gfx->setTextColor(CYAN);
    gfx->setTextSize(2);

    gfx->setCursor(120, 130);
    gfx->print("KM/H");
}

void loop()
{
}