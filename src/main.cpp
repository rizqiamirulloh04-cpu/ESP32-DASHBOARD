#include <Arduino.h>
#include <Arduino_GFX_Library.h>

#define TFT_BL   22

#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS   14
#define TFT_DC   15
#define TFT_RST  21

// ======================
// LCD CONFIG
// ======================

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

// ======================
// GLOBAL
// ======================

int speedValue = 0;
int speedDir = 1;

int steering = 0;
int steeringDir = 1;

int rpmBar = 0;

// ======================
// DRAW UI
// ======================

void drawStaticUI()
{
    gfx->fillScreen(0x0000);

    // ===== TOP BAR =====

    gfx->setTextSize(1);

    // WIFI
    gfx->setTextColor(0x07FF);
    gfx->setCursor(5, 5);
    gfx->println("WIFI");

    // BATTERY
    gfx->drawRect(130, 4, 24, 10, 0x07E0);
    gfx->fillRect(154, 7, 2, 4, 0x07E0);
    gfx->fillRect(132, 6, 18, 6, 0x07E0);

    // ===== TITLE =====

    gfx->setTextColor(0xFFFF);
    gfx->setTextSize(1);

    gfx->setCursor(55, 18);
    gfx->println("RC DASH");

    // ===== SPEED LABEL =====

    gfx->setTextColor(0x07FF);
    gfx->setCursor(63, 40);
    gfx->println("SPEED");

    // ===== RPM TEXT =====

    gfx->setTextColor(0xFD20);
    gfx->setCursor(5, 138);
    gfx->println("RPM");

    // ===== RGB STYLE BAR =====

    gfx->fillRect(10, 150, 45, 10, 0xF800);
    gfx->fillRect(63, 150, 45, 10, 0x07E0);
    gfx->fillRect(116, 150, 45, 10, 0x001F);
}

// ======================
// DRAW SPEED
// ======================

void drawSpeed(int speed)
{
    // clear area
    gfx->fillRect(35, 55, 110, 45, 0x0000);

    gfx->setTextColor(0xFFFF);
    gfx->setTextSize(4);

    gfx->setCursor(42, 58);

    if (speed < 10)
    {
        gfx->print("00");
    }
    else if (speed < 100)
    {
        gfx->print("0");
    }

    gfx->print(speed);

    gfx->setTextSize(1);

    gfx->setCursor(72, 102);
    gfx->setTextColor(0xFD20);
    gfx->println("KM/H");
}

// ======================
// DRAW STEERING
// ======================

void drawSteering(int value)
{
    // clear left/right area
    gfx->fillRect(0, 55, 35, 45, 0x0000);
    gfx->fillRect(137, 55, 35, 45, 0x0000);

    // LEFT
    if (value < -20)
    {
        gfx->fillTriangle(
            8, 78,
            28, 65,
            28, 91,
            0x07E0
        );
    }

    // RIGHT
    if (value > 20)
    {
        gfx->fillTriangle(
            164, 78,
            144, 65,
            144, 91,
            0x07E0
        );
    }
}

// ======================
// DRAW RPM BAR
// ======================

void drawRPM(int value)
{
    gfx->fillRect(10, 122, 152, 10, 0x2104);

    int width = map(value, 0, 100, 0, 152);

    uint16_t color = 0x07E0;

    if (value > 50)
    {
        color = 0xFFE0;
    }

    if (value > 80)
    {
        color = 0xF800;
    }

    gfx->fillRect(10, 122, width, 10, color);
}

// ======================
// SETUP
// ======================

void setup()
{
    // Backlight redup
    ledcAttach(TFT_BL, 5000, 8);
    ledcWrite(TFT_BL, 40);

    gfx->begin();

    gfx->invertDisplay(false);

    drawStaticUI();
}

// ======================
// LOOP
// ======================

void loop()
{
    // ===== SPEED =====

    speedValue += speedDir * 2;

    if (speedValue >= 120)
    {
        speedDir = -1;
    }

    if (speedValue <= 0)
    {
        speedDir = 1;
    }

    // ===== STEERING =====

    steering += steeringDir * 6;

    if (steering >= 100)
    {
        steeringDir = -1;
    }

    if (steering <= -100)
    {
        steeringDir = 1;
    }

    // ===== RPM =====

    rpmBar = map(speedValue, 0, 120, 0, 100);

    // ===== DRAW =====

    drawSpeed(speedValue);

    drawSteering(steering);

    drawRPM(rpmBar);

    delay(20);
}