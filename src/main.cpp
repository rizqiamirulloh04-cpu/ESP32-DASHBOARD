#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <math.h>

// =====================================================
// LCD PINS
// =====================================================

#define TFT_BL   22

#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS   14
#define TFT_DC   15
#define TFT_RST  21

// =====================================================
// DISPLAY
// =====================================================

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

// =====================================================
// COLORS
// =====================================================

#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define CYAN    0x07FF
#define GREEN   0x07E0
#define YELLOW  0xFFE0
#define GRAY    0x4208
#define DARK    0x18C3

// =====================================================
// VARIABLES
// =====================================================

int speedValue = 0;
bool upDir = true;

// =====================================================
// DRAW ARC
// =====================================================

void drawArc()
{
    int cx = 160;
    int cy = 92;

    // Arc lebih besar
    int radius = 58;

    // Background arc
    for (int i = -150; i <= -30; i += 2)
    {
        float rad = i * 0.0174533;

        int x1 = cx + cos(rad) * radius;
        int y1 = cy + sin(rad) * radius;

        int x2 = cx + cos(rad) * (radius - 8);
        int y2 = cy + sin(rad) * (radius - 8);

        gfx->drawLine(x1, y1, x2, y2, DARK);
    }

    // Active arc
    int active = map(speedValue, 0, 120, -150, -30);

    for (int i = -150; i <= active; i += 2)
    {
        float rad = i * 0.0174533;

        int x1 = cx + cos(rad) * radius;
        int y1 = cy + sin(rad) * radius;

        int x2 = cx + cos(rad) * (radius - 8);
        int y2 = cy + sin(rad) * (radius - 8);

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

// =====================================================
// DRAW NEEDLE
// =====================================================

void drawNeedle()
{
    int cx = 160;
    int cy = 92;

    int angle = map(speedValue, 0, 120, -150, -30);

    float rad = angle * 0.0174533;

    // Jarum lebih pendek
    int r = 30;

    int x = cx + cos(rad) * r;
    int y = cy + sin(rad) * r;

    gfx->drawLine(cx, cy, x, y, RED);

    gfx->fillCircle(cx, cy, 3, WHITE);
}

// =====================================================
// DRAW SCALE
// =====================================================

void drawScale()
{
    gfx->setTextSize(1);
    gfx->setTextColor(WHITE);

    gfx->setCursor(58, 96);
    gfx->print("0");

    gfx->setCursor(102, 34);
    gfx->print("40");

    gfx->setCursor(198, 34);
    gfx->print("80");

    gfx->setCursor(252, 96);
    gfx->print("120");
}

// =====================================================
// DRAW SPEED
// =====================================================

void drawSpeed()
{
    gfx->setTextColor(WHITE);

    gfx->setTextSize(4);

    // Posisi angka lebih center
    if (speedValue < 10)
    {
        gfx->setCursor(134, 64);
    }
    else if (speedValue < 100)
    {
        gfx->setCursor(122, 64);
    }
    else
    {
        gfx->setCursor(110, 64);
    }

    // Leading zero
    if (speedValue < 10)
        gfx->print("00");

    else if (speedValue < 100)
        gfx->print("0");

    gfx->print(speedValue);

    // KM/H turun sedikit
    gfx->setTextSize(2);
    gfx->setTextColor(CYAN);

    gfx->setCursor(122, 112);
    gfx->print("KM/H");
}

// =====================================================
// DRAW STATUS
// =====================================================

void drawStatus()
{
    gfx->setTextSize(1);

    gfx->setTextColor(GREEN);

    gfx->setCursor(8, 8);
    gfx->print("SPORT");

    gfx->setCursor(262, 8);
    gfx->print("READY");
}

// =====================================================
// DRAW RPM BAR
// =====================================================

void drawRPM()
{
    // Background
    gfx->fillRoundRect(35, 145, 250, 10, 4, GRAY);

    // Value
    int bar = map(speedValue, 0, 120, 0, 250);

    uint16_t color;

    if (speedValue < 70)
        color = CYAN;
    else if (speedValue < 100)
        color = YELLOW;
    else
        color = RED;

    gfx->fillRoundRect(35, 145, bar, 10, 4, color);
}

// =====================================================
// MAIN DRAW
// =====================================================

void drawDashboard()
{
    gfx->fillScreen(BLACK);

    drawStatus();
    drawArc();
    drawScale();
    drawNeedle();
    drawSpeed();
    drawRPM();
}

// =====================================================
// SETUP
// =====================================================

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

// =====================================================
// LOOP
// =====================================================

void loop()
{
    drawDashboard();

    if (upDir)
    {
        speedValue++;

        if (speedValue >= 120)
        {
            upDir = false;
        }
    }
    else
    {
        speedValue--;

        if (speedValue <= 0)
        {
            upDir = true;
        }
    }

    delay(22);
}