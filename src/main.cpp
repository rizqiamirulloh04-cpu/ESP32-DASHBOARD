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
    static int steering = -100;
    static int steeringDir = 1;

    // ===== SPEED =====

    speedValue += dir * 2;

    if (speedValue >= 120)
    {
        dir = -1;
    }

    if (speedValue <= 0)
    {
        dir = 1;
    }

    // ===== STEERING ANIMATION =====

    steering += steeringDir * 8;

    if (steering >= 100)
    {
        steeringDir = -1;
    }

    if (steering <= -100)
    {
        steeringDir = 1;
    }

    // ===== CLEAR SPEED AREA =====

    gfx->fillRect(60, 35, 210, 60, 0x0000);

    // ===== CLEAR ARROW AREA =====

    gfx->fillRect(0, 45, 55, 50, 0x0000);
    gfx->fillRect(265, 45, 55, 50, 0x0000);

    // ===== LEFT ARROW =====

    uint16_t leftColor = 0x4208;

    if (steering < -20)
    {
        leftColor = 0x07E0;
    }

    gfx->fillTriangle(
        20, 70,
        45, 55,
        45, 85,
        leftColor
    );

    // ===== RIGHT ARROW =====

    uint16_t rightColor = 0x4208;

    if (steering > 20)
    {
        rightColor = 0x07E0;
    }

    gfx->fillTriangle(
        300, 70,
        275, 55,
        275, 85,
        rightColor
    );

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

    gfx->fillRect(40, 145, 240, 12, 0x2104);

    gfx->fillRect(40, 145, bar, 12, 0xF800);

    delay(20);
}