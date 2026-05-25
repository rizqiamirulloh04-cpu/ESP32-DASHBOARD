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

    // KM/H static
    gfx->setTextColor(0x07FF);
    gfx->setTextSize(3);

    gfx->setCursor(105, 110);
    gfx->println("KM/H");

    // Bar background static
    gfx->fillRect(40, 145, 240, 12, 0x2104);
}

void loop()
{
    // ===== SPEED UPDATE =====

    speedValue += dir * 2;

    if (speedValue >= 120)
    {
        dir = -1;
    }

    if (speedValue <= 0)
    {
        dir = 1;
    }

    // ===== CLEAR SPEED ONLY =====

    gfx->fillRect(60, 35, 210, 60, 0x0000);

    // ===== DRAW SPEED =====

    gfx->setTextColor(0xFFFF);
    gfx->setTextSize(6);

    if (speedValue < 10)
    {
        gfx->setCursor(95, 45);
        gfx->print("00");
    }
    else if (speedValue < 100)
    {
        gfx->setCursor(95, 45);
        gfx->print("0");
    }
    else
    {
        gfx->setCursor(75, 45);
    }

    gfx->print(speedValue);

    // ===== RPM BAR =====

    int bar = map(speedValue, 0, 120, 0, 240);

    // clear previous bar
    gfx->fillRect(40, 145, 240, 12, 0x2104);

    // draw new bar
    gfx->fillRect(40, 145, bar, 12, 0xF800);

    delay(20);
}