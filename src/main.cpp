#include <Arduino.h>
#include <Arduino_GFX_Library.h>

#define TFT_BL   15

#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_DC   8
#define TFT_RST  9
#define TFT_CS   14

Arduino_DataBus *bus = new Arduino_ESP32SPI(
    TFT_DC,
    TFT_CS,
    TFT_SCLK,
    TFT_MOSI,
    GFX_NOT_DEFINED
);

Arduino_GFX *gfx = new Arduino_ST7789(
    bus,
    TFT_RST,
    1,      // rotation
    true,   // IPS
    172,    // width
    320,    // height
    34,     // x offset
    0       // y offset
);

void setup()
{
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    gfx->begin();
    gfx->fillScreen(BLACK);

    gfx->setTextSize(3);
    gfx->setTextColor(WHITE);

    gfx->setCursor(20, 20);
    gfx->println("RC DASH");

    // Border test
    gfx->drawRect(0, 0, 172, 320, RED);

    // Center line
    gfx->drawLine(0, 160, 172, 160, GREEN);

    // Speed text
    gfx->setTextSize(6);
    gfx->setCursor(25, 120);
    gfx->println("046");

    // Bottom bars
    gfx->fillRect(10, 280, 45, 20, RED);
    gfx->fillRect(63, 280, 45, 20, GREEN);
    gfx->fillRect(116, 280, 45, 20, BLUE);
}

void loop()
{
}