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
    ledcAttach(TFT_BL, 5000, 8);
    ledcWrite(TFT_BL, 35);

    gfx->begin();
    gfx->invertDisplay(false);
    gfx->fillScreen(0x0000);
}

void drawUI()
{
    gfx->fillScreen(0x0000);

    // Title
    gfx->setTextColor(0x07E0);
    gfx->setTextSize(2);

    gfx->setCursor(10, 8);
    gfx->println("RC DASHBOARD");

    // SPEED
    gfx->setTextSize(5);

    if (speedValue < 10)
        gfx->setCursor(52, 45);
    else if (speedValue < 100)
        gfx->setCursor(38, 45);
    else
        gfx->setCursor(20, 45);

    gfx->setTextColor(0xFFFF);
    gfx->println(speedValue);

    // KMH
    gfx->setTextSize(2);
    gfx->setCursor(60, 100);
    gfx->println("KM/H");

    // Speed bar background
    gfx->drawRect(20, 130, 132, 12, 0xFFFF);

    // Speed bar fill
    int bar = map(speedValue, 0, 120, 0, 128);

    gfx->fillRect(22, 132, bar, 8, 0xF800);

    // RGB bottom
    gfx->fillRect(15, 150, 40, 10, 0xF800);
    gfx->fillRect(65, 150, 40, 10, 0x07E0);
    gfx->fillRect(115, 150, 40, 10, 0x001F);
}

void loop()
{
    speedValue += dir;

    if (speedValue >= 120)
        dir = -1;

    if (speedValue <= 0)
        dir = 1;

    drawUI();

    delay(30);
}