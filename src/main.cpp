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
// PIN LCD
// ==========================

#define TFT_BL   15

#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_DC   8
#define TFT_RST  9
#define TFT_CS   14

// ==========================
// BUS SPI
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
    0       // row offset
);

// ==========================

void setup()
{
    Serial.begin(115200);

    // BACKLIGHT
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    // INIT DISPLAY
    gfx->begin();

    gfx->fillScreen(BLACK);

    delay(500);

    // ==========================
    // TEST COLOR
    // ==========================

    gfx->fillScreen(RED);
    delay(1000);

    gfx->fillScreen(GREEN);
    delay(1000);

    gfx->fillScreen(BLUE);
    delay(1000);

    gfx->fillScreen(BLACK);

    // ==========================
    // TEST DRAW
    // ==========================

    gfx->drawRect(0, 0, 172, 320, WHITE);

    gfx->drawFastHLine(36, 26, 100, CYAN);

    gfx->fillTriangle(
        86, 12,
        78, 24,
        94, 24,
        GREEN
    );

    // LEFT BAR
    gfx->fillRoundRect(
        22,
        48,
        6,
        110,
        3,
        RED
    );

    // RIGHT BAR
    gfx->fillRoundRect(
        144,
        48,
        6,
        110,
        3,
        CYAN
    );

    // SPEED
    gfx->setTextColor(WHITE);
    gfx->setTextSize(4);

    gfx->setCursor(42, 82);
    gfx->print("000");

    // KM/H
    gfx->setTextColor(CYAN);
    gfx->setTextSize(2);

    gfx->setCursor(52, 128);
    gfx->print("KM/H");
}

void loop()
{
}