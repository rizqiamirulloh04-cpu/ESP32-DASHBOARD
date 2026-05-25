#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <math.h>

// ================= LCD PINS =================

#define TFT_BL   22

#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS   14
#define TFT_DC   15
#define TFT_RST  21

// ================= DISPLAY =================

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

// ================= COLORS =================

#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define CYAN    0x07FF
#define GREEN   0x07E0
#define YELLOW  0xFFE0
#define GRAY    0x2104
#define DARK    0x1082

// ================= VARIABLES =================

int speedValue = 0;
bool upDir = true;

// ================= DRAW ARC =================

void drawArcMeter()
{
    int cx = 160;
    int cy = 120;
    int r  = 80;

    // Background arc
    for (int i = -140; i <= -40; i += 2)
    {
        float rad = i * 0.0174533;

        int x1 = cx + cos(rad) * r;
        int y1 = cy + sin(rad) * r;

        int x2 = cx + cos(rad) * (r - 12);
        int y2 = cy + sin(rad) * (r - 12);

        gfx->drawLine(x1, y1, x2, y2, DARK);
    }

    // Active arc
    int activeAngle = map(speedValue, 0, 120, -140, -40);

    for (int i = -140; i <= activeAngle; i += 2)
    {
        float rad = i * 0.0174533;

        int x1 = cx + cos(rad) * r;
        int y1 = cy + sin(rad) * r;

        int x2 = cx + cos(rad) * (r - 12);
        int y2 = cy + sin(rad) * (r - 12);

        uint16_t color;

        if (i < -90)
            color = CYAN;
        else if (i < -60)
            color = YELLOW;
        else
            color = RED;

        gfx->drawLine(x1, y1, x2, y2, color);
    }
}

// ================= DRAW NEEDLE =================

void drawNeedle()
{
    int cx = 160;
    int cy = 120;

    int angle = map(speedValue, 0, 120, -140, -40);

    float rad = angle * 0.0174533;

    int r = 60;

    int x = cx + cos(rad) * r;
    int y = cy + sin(rad) * r;

    gfx->drawLine(cx, cy, x, y, RED);

    gfx->fillCircle(cx, cy, 4, WHITE);
}

// ================= DRAW SPEED =================

void drawSpeed()
{
    gfx->setTextColor(WHITE);
    gfx->setTextSize(5);

    if (speedValue < 10)
        gfx->setCursor(126, 55);
    else if (speedValue < 100)
        gfx->setCursor(106, 55);
    else
        gfx->setCursor(86, 55);

    gfx->print(speedValue);

    gfx->setTextSize(2);
    gfx->setTextColor(CYAN);
    gfx->setCursor(122, 100);
    gfx->print("KM/H");
}

// ================= DRAW SCALE =================

void drawScale()
{
    gfx->setTextSize(1);
    gfx->setTextColor(WHITE);

    gfx->setCursor(35, 100);
    gfx->print("0");

    gfx->setCursor(70, 45);
    gfx->print("40");

    gfx->setCursor(145, 20);
    gfx->print("80");

    gfx->setCursor(255, 45);
    gfx->print("120");
}

// ================= DRAW BAR =================

void drawBar()
{
    gfx->fillRect(35, 145, 250, 10, GRAY);

    int bar = map(speedValue, 0, 120, 0, 250);

    gfx->fillRect(35, 145, bar, 10, RED);
}

// ================= MAIN UI =================

void drawDashboard()
{
    gfx->fillScreen(BLACK);

    // Top text
    gfx->setTextColor(GREEN);
    gfx->setTextSize(1);

    gfx->setCursor(10, 10);
    gfx->print("SPORT");

    gfx->setCursor(270, 10);
    gfx->print("READY");

    drawArcMeter();
    drawNeedle();
    drawScale();
    drawSpeed();
    drawBar();
}

// ================= SETUP =================

void setup()
{
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    Serial.begin(115200);

    gfx->begin();
    gfx->setRotation(1);
    gfx->invertDisplay(false);

    gfx->fillScreen(BLACK);
}

// ================= LOOP =================

void loop()
{
    drawDashboard();

    if (upDir)
    {
        speedValue++;

        if (speedValue >= 120)
            upDir = false;
    }
    else
    {
        speedValue--;

        if (speedValue <= 0)
            upDir = true;
    }

    delay(25);
}