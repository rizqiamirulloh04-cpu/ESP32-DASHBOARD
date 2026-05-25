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

int speedValue = 0;
int dir = 1;

void setup()
{
    // Backlight redup
    ledcAttach(TFT_BL, 5000, 8);
    ledcWrite(TFT_BL, 35);

    gfx->begin();

    gfx->invertDisplay(false);

    gfx->fillScreen(0x0000);

    // ===== TITLE =====

    // kosongkan title

    // ===== RGB BAR =====

    gfx->fillRect(55, 150, 55, 10, 0xF800);
    gfx->fillRect(132, 150, 55, 10, 0x07E0);
    gfx->fillRect(209, 150, 55, 10, 0x001F);
}

void loop()
{
    // Hapus area speed
    gfx->fillRect( 0,  0, 320,130, 0x0000);

    // Speed animasi
    speedValue += dir * 2;

    if (speedValue >= 120)
    {
        dir = -1;
    }

    if (speedValue <= 0)
    {
        dir = 1;
    }

    // ===== SPEED =====

    gfx->setTextColor(0xFFFF);
    gfx->setTextSize(6);

    if (speedValue < 10)
    {
        gfx->setCursor(85, 45);
        gfx->print("00");
    }
    else if (speedValue < 100)
    {
        gfx->setCursor(95, 55);
        gfx->print("0");
    }
    else
    {
        gfx->setCursor(75, 55);
    }

    gfx->print(speedValue);

    // ===== KM/H =====

    gfx->setTextColor(0x07FF);
    gfx->setTextSize(3);

    gfx->setCursor(105, 110);
    gfx->println("KM/H");

    // ===== SPEED BAR =====

    gfx->fillRect(40, 135, 240, 12, 0x2104);

    int bar = map(speedValue, 0, 120, 0, 240);

    gfx->fillRect(40, 135, bar, 12, 0xF800);

    delay(25);
}