#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// ======================
// WAVESHARE ESP32-C6 LCD 1.47"
// ======================

#define TFT_BL   15

#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_DC   8
#define TFT_RST  9
#define TFT_CS   14

// ======================
// BUS
// ======================

Arduino_DataBus *bus = new Arduino_ESP32SPI(
    TFT_DC,
    TFT_CS,
    TFT_SCLK,
    TFT_MOSI,
    GFX_NOT_DEFINED
);

// ======================
// DISPLAY
// ======================

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

// ======================

void setup()
{
    Serial.begin(115200);

    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    gfx->begin();

    // brightness
    gfx->fillScreen(BLACK);

    delay(500);

    // TEST COLORS
    gfx->fillScreen(RED);
    delay(500);

    gfx->fillScreen(GREEN);
    delay(500);

    gfx->fillScreen(BLUE);
    delay(500);

    gfx->fillScreen(BLACK);

    // ======================
    // UI TEST
    // ======================

    gfx->drawRect(0, 0, 320, 172, WHITE);

    gfx->drawFastHLine(70, 20, 180, CYAN);

    gfx->fillTriangle(
        160, 8,
        150, 20,
        170, 20,
        GREEN
    );

    gfx->fillRoundRect(40, 40, 8, 100, 3, RED);
    gfx->fillRoundRect(272, 40, 8, 100, 3, CYAN);

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