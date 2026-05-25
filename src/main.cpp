#include <Arduino.h>
#include <Arduino_GFX_Library.h>

#define TFT_BL   15

#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_DC   8
#define TFT_RST  9
#define TFT_CS   14

#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
#define CYAN    0x07FF

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
    0,
    true,
    172,
    320,
    34,
    0
);

void setup()
{
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    gfx->begin();

    gfx->fillScreen(BLACK);

    // TOP LINE
    gfx->drawFastHLine(38, 26, 64, CYAN);

    // TRIANGLE
    gfx->fillTriangle(
        70, 12,
        62, 24,
        78, 24,
        GREEN
    );

    // LEFT BAR
    gfx->fillRoundRect(
        22,
        48,
        5,
        90,
        3,
        RED
    );

    // RIGHT BAR
    gfx->fillRoundRect(
        145,
        48,
        5,
        90,
        3,
        CYAN
    );

    // SPEED
    gfx->setTextColor(WHITE);
    gfx->setTextSize(4);

    gfx->setCursor(38, 76);
    gfx->print("000");

    // KM/H
    gfx->setTextColor(CYAN);
    gfx->setTextSize(2);

    gfx->setCursor(50, 132);
    gfx->print("KM/H");
}

void loop()
{
}