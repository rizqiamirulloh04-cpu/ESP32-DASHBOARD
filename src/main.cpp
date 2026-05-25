#include <Arduino.h>
#include <Arduino_GFX_Library.h>

#define TFT_BL   22

#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS   14
#define TFT_DC   15
#define TFT_RST  21

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
    3,
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
    // Backlight redup
    ledcAttach(TFT_BL, 5000, 8);
    ledcWrite(TFT_BL, 40);

    gfx->begin();

    gfx->invertDisplay(false);

    gfx->fillScreen(0x0000);

gfx->setTextColor(0x07E0);

gfx->fillRect(20, 140, 80, 20, 0xF800);
gfx->fillRect(120, 140, 80, 20, 0x07E0);
gfx->fillRect(220, 140, 80, 20, 0x001F);

void loop()
{
}