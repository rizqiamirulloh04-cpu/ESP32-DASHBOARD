// =====================================================
// MINI OEM SPEED HUD
// ESP32-C6 + ST7789 + Arduino_GFX
// 320x170 Landscape
// =====================================================

#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <math.h>

// =====================================================
// PINS
// =====================================================

#define TFT_BL   22

#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS   14
#define TFT_DC   15
#define TFT_RST  21

// =====================================================
// COLORS
// =====================================================

#define BLACK         0x0000
#define UI_WHITE      0xE71C
#define SPEED_WHITE   0xFFFF
#define DARK          0x2104
#define CYAN          0x07FF
#define GREEN         0x07E0
#define YELLOW        0xFFE0
#define RED           0xF800
#define GRAY          0x52AA

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
    170,
    320
);

// =====================================================
// VARIABLES
// =====================================================

float displaySpeed = 0;
float targetSpeed = 0;

unsigned long signalTimer = 0;
int signalFrame = 0;

// =====================================================
// PANEL
// =====================================================

void drawPanel()
{
    gfx->drawRoundRect(
        55,
        28,
        210,
        100,
        12,
        DARK
    );
}

// =====================================================
// SCALE
// =====================================================

void drawScale()
{
    gfx->setTextSize(1);
    gfx->setTextColor(UI_WHITE);

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
// ARC BACKGROUND
// =====================================================

void drawArcBackground()
{
    int cx = 160;
    int cy = 84;
    int radius = 58;

    for (int i = -150; i <= -30; i += 1)
    {
        float rad = i * 0.0174533;

        int x1 = cx + cos(rad) * radius;
        int y1 = cy + sin(rad) * radius;

        int x2 = cx + cos(rad) * (radius - 14);
        int y2 = cy + sin(rad) * (radius - 14);

        gfx->drawLine(x1, y1, x2, y2, DARK);
    }
}

// =====================================================
// ACTIVE ARC
// =====================================================

void drawActiveArc()
{
    int cx = 160;
    int cy = 84;
    int radius = 58;

    int endAngle = map(displaySpeed, 0, 120, -150, -30);

    for (int i = -150; i <= endAngle; i += 1)
    {
        float rad = i * 0.0174533;

        int x1 = cx + cos(rad) * radius;
        int y1 = cy + sin(rad) * radius;

        int x2 = cx + cos(rad) * (radius - 14);
        int y2 = cy + sin(rad) * (radius - 14);

        gfx->drawLine(x1, y1, x2, y2, CYAN);
    }
}

// =====================================================
// NEEDLE
// =====================================================

void drawNeedle()
{
    int cx = 160;
    int cy = 84;

    float angle = map(displaySpeed, 0, 120, -150, -30);

    float rad = angle * 0.0174533;

    int r = 30;

    int x = cx + cos(rad) * r;
    int y = cy + sin(rad) * r;

    gfx->drawLine(cx, cy, x, y, RED);

    gfx->fillCircle(cx, cy, 3, UI_WHITE);
}

// =====================================================
// SPEED
// =====================================================

void drawSpeed()
{
    gfx->fillRect(
        90,
        45,
        140,
        80,
        BLACK
    );

    gfx->setTextSize(4);

    char speedText[10];
    sprintf(speedText, "%03d", (int)displaySpeed);

    int x = 118;
    int y = 58;

    // shadow
    gfx->setTextColor(DARK);

    gfx->setCursor(x + 2, y + 2);
    gfx->print(speedText);

    // main text
    gfx->setTextColor(SPEED_WHITE);

    gfx->setCursor(x, y);
    gfx->print(speedText);

    // KM/H
    gfx->setTextSize(2);

    gfx->setTextColor(UI_WHITE);

    gfx->setCursor(122, 112);
    gfx->print("KM/H");
}

// =====================================================
// RPM BAR
// =====================================================

void drawRPM()
{
    gfx->fillRect(
        35,
        145,
        250,
        8,
        BLACK
    );

    // background
    gfx->fillRoundRect(
        35,
        145,
        250,
        8,
        4,
        DARK
    );

    int width = map(displaySpeed, 0, 120, 0, 250);

    gfx->fillRoundRect(
        35,
        145,
        width,
        8,
        4,
        CYAN
    );
}

// =====================================================
// BATTERY
// =====================================================

void drawBattery(int percent)
{
    int x = 12;
    int y = 32;

    gfx->fillRect(0, 28, 90, 24, BLACK);

    gfx->drawRoundRect(
        x,
        y,
        24,
        12,
        3,
        UI_WHITE
    );

    gfx->fillRect(
        x + 24,
        y + 3,
        3,
        6,
        UI_WHITE
    );

    int fill = map(percent, 0, 100, 0, 20);

    uint16_t levelColor;

    if (percent > 60)
        levelColor = GREEN;
    else if (percent > 30)
        levelColor = YELLOW;
    else
        levelColor = RED;

    gfx->fillRoundRect(
        x + 2,
        y + 2,
        fill,
        8,
        2,
        levelColor
    );

    gfx->setTextSize(1);
    gfx->setTextColor(UI_WHITE);

    gfx->setCursor(34, 35);
    gfx->print(percent);
    gfx->print("%");
}

// =====================================================
// LAMP
// =====================================================

void drawLamp(bool active)
{
    gfx->fillRect(250, 28, 50, 24, BLACK);

    if (active)
        gfx->setTextColor(YELLOW);
    else
        gfx->setTextColor(DARK);

    gfx->setTextSize(2);

    gfx->setCursor(270, 33);
    gfx->print("*");
}

// =====================================================
// SIGNALS
// =====================================================

void drawSignals(int frame)
{
    gfx->fillRect(0, 0, 320, 24, BLACK);

    gfx->setTextSize(2);

    // LEFT
    gfx->setTextColor(frame == 0 ? YELLOW : DARK);
    gfx->setCursor(10, 4);
    gfx->print("<");

    gfx->setTextColor(frame == 1 ? YELLOW : DARK);
    gfx->setCursor(24, 4);
    gfx->print("<");

    gfx->setTextColor(frame == 2 ? YELLOW : DARK);
    gfx->setCursor(38, 4);
    gfx->print("<");

    // RIGHT
    gfx->setTextColor(frame == 0 ? YELLOW : DARK);
    gfx->setCursor(280, 4);
    gfx->print(">");

    gfx->setTextColor(frame == 1 ? YELLOW : DARK);
    gfx->setCursor(266, 4);
    gfx->print(">");

    gfx->setTextColor(frame == 2 ? YELLOW : DARK);
    gfx->setCursor(252, 4);
    gfx->print(">");
}

// =====================================================
// STATIC UI
// =====================================================

void drawStaticUI()
{
    gfx->fillScreen(BLACK);

    drawPanel();

    drawScale();

    drawArcBackground();

    drawBattery(82);

    drawLamp(true);
}

// =====================================================
// DASHBOARD
// =====================================================

void drawDashboard()
{
    drawActiveArc();

    drawNeedle();

    drawSpeed();

    drawRPM();
}

// =====================================================
// SETUP
// =====================================================

void setup()
{
    Serial.begin(115200);

    // backlight PWM
    pinMode(TFT_BL, OUTPUT);

    ledcAttach(TFT_BL, 5000, 8);

    // brightness
    ledcWrite(TFT_BL, 85);

    gfx->begin();

    gfx->setRotation(1);

    gfx->invertDisplay(false);

    drawStaticUI();
}

// =====================================================
// LOOP
// =====================================================

void loop()
{
    // demo animation
    static bool up = true;

    if (up)
        targetSpeed += 1;
    else
        targetSpeed -= 1;

    if (targetSpeed >= 120)
        up = false;

    if (targetSpeed <= 0)
        up = true;

    // smoothing
    displaySpeed +=
        (targetSpeed - displaySpeed) * 0.22;

    // dashboard
    drawDashboard();

    // signal animation
    if (millis() - signalTimer > 120)
    {
        signalTimer = millis();

        signalFrame++;

        if (signalFrame > 2)
            signalFrame = 0;

        drawSignals(signalFrame);
    }

    delay(16);
}