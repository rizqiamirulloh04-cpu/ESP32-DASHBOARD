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
    // Backlight
    ledcAttach(TFT_BL, 5000, 8);
    ledcWrite(TFT_BL, 40);

    gfx->begin();

    gfx->invertDisplay(false);

    gfx->fillScreen(0x0000);

    // ===== TITLE =====

    gfx->setTextColor(0x07E0);
    gfx->setTextSize(2);

    gfx->setCursor(20, 10);
    gfx->println("RC DASHBOARD");

    // ===== RGB BAR =====

    gfx->fillRect(15, 145, 40, 10, 0xF800);
    gfx->fillRect(65, 145, 40, 10, 0x07E0);
    gfx->fillRect(115, 145, 40, 10, 0x001F);
}

void loop()
{
    // Hapus area speed
    gfx->fillRect(25, 45, 120, 60, 0x0000);

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
    gfx->setTextSize(4);

    gfx->setCursor(35, 55);

    if (speedValue < 10)
    {
        gfx->print("00");
    }
    else if (speedValue < 100)
    {
        gfx->print("0");
    }

    gfx->print(speedValue);

    // ===== KMH =====

    gfx->setTextColor(0x07FF);
    gfx->setTextSize(2);

    gfx->setCursor(55, 105);
    gfx->println("KM/H");

    // ===== SIMPLE RPM BAR =====

    gfx->fillRect(10, 125, 152, 10, 0x2104);

    int bar = map(speedValue, 0, 120, 0, 152);

    gfx->fillRect(10, 125, bar, 10, 0xF800);

    delay(30);
}