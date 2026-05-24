#include <Arduino.h>
#include <Arduino_GFX_Library.h>

#define TFT_BL    15

#define TFT_MOSI  6
#define TFT_SCLK  7
#define TFT_DC    8
#define TFT_CS    14
#define TFT_RST   9

#define BLACK 0x0000
#define WHITE 0xFFFF
#define RED   0xF800

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
    1,
    true,
    172,
    320,
    34,
    0,
    34,
    0
);

void setup()
{
    Serial.begin(115200);

    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    gfx->begin();

    gfx->fillScreen(BLACK);

    gfx->setTextColor(WHITE);
    gfx->setTextSize(2);

    gfx->setCursor(20, 40);
    gfx->println("DISPLAY OK");

    gfx->drawRect(0, 0, 320, 172, RED);
}

void loop()
{
}