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
    ledcWrite(TFT_BL, 80);

    gfx->begin();

    gfx->invertDisplay(false);

    // Background hitam
    gfx->fillScreen(0x0000);

    // Text hijau
    gfx->setTextColor(0x07E0);
    gfx->setTextSize(3);

    gfx->setCursor(30, 40);
    gfx->println("ESP32-C6");

    gfx->setCursor(30, 90);
    gfx->println("LANDSCAPE");

    // Kotak warna
    gfx->fillRect(20, 140, 80, 20, 0xF800); // merah
    gfx->fillRect(120, 140, 80, 20, 0x07E0); // hijau
    gfx->fillRect(220, 140, 80, 20, 0x001F); // biru
}

void loop()
{
}