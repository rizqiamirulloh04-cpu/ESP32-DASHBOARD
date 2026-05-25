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

#define BLACK        0x0000

// Soft UI white
#define WHITE        0xE71C

// Bright speed number
#define SPEED_WHITE  0xFFFF

#define RED          0xF800
#define CYAN         0x07FF
#define GREEN        0x07E0
#define YELLOW       0xFFE0

#define GRAY         0x4208
#define DARK         0x18C3

// =====================================================
// VARIABLES
// =====================================================

float displaySpeed = 0;
float targetSpeed  = 0;

bool upDir = true;

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
// STATUS
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
// SCALE
// =====================================================

void drawScale()
{
    gfx->setTextSize(1);

    gfx->setTextColor(WHITE);

    gfx->setCursor(58, 88);
    gfx->print("0");

    gfx->setCursor(102, 28);
    gfx->print("40");

    gfx->setCursor(198, 28);
    gfx->print("80");

    gfx->setCursor(252, 88);
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

        gfx->drawLine(
            x1,
            y1,
            x2,
            y2,
            DARK
        );
    }
}

// =====================================================
// ACTIVE ARC
// =====================================================

void drawActiveArc()
{
    gfx->fillRect(
        82,
        18,
        160,
        60,
        BLACK
    );

    drawArcBackground();

    drawScale();

    int cx = 160;
    int cy = 84;

    int radius = 58;

    int active = map(
        (int)displaySpeed,
        0,
        120,
        -150,
        -30
    );

    for (int i = -150; i <= active; i += 1)
    {
        float rad = i * 0.0174533;

        int x1 = cx + cos(rad) * radius;
        int y1 = cy + sin(rad) * radius;

        int x2 = cx + cos(rad) * (radius - 14);
        int y2 = cy + sin(rad) * (radius - 14);

        uint16_t color;

        if (i < -90)
            color = CYAN;

        else if (i < -60)
            color = YELLOW;

        else
            color = RED;

        gfx->drawLine(
            x1,
            y1,
            x2,
            y2,
            color
        );
    }
}

// =====================================================
// NEEDLE
// =====================================================

void drawNeedle()
{
    gfx->fillCircle(
        160,
        84,
        35,
        BLACK
    );

    int cx = 160;
    int cy = 84;

    int angle = map(
        (int)displaySpeed,
        0,
        120,
        -150,
        -30
    );

    float rad = angle * 0.0174533;

    int r = 30;

    int x = cx + cos(rad) * r;
    int y = cy + sin(rad) * r;

    gfx->drawLine(
        cx,
        cy,
        x,
        y,
        RED
    );

    gfx->fillCircle(
        cx,
        cy,
        3,
        SPEED_WHITE
    );

    gfx->drawCircle(
        cx,
        cy,
        6,
        CYAN
    );
}

// =====================================================
// SPEED TEXT
// =====================================================

void drawSpeed()
{
    gfx->fillRect(
        100,
        52,
        120,
        74,
        BLACK
    );

    int speed = (int)displaySpeed;

    char speedText[4];

    sprintf(speedText, "%03d", speed);

    int x;

    if (speed < 10)
        x = 134;

    else if (speed < 100)
        x = 122;

    else
        x = 110;

    int y = 68;

    gfx->setTextSize(4);

    // =================================================
    // SHADOW
    // =================================================

    gfx->setTextColor(DARK);

    gfx->setCursor(x + 2, y + 2);
    gfx->print(speedText);

    // =================================================
    // MAIN SPEED TEXT
    // =================================================

    gfx->setTextColor(SPEED_WHITE);

    gfx->setCursor(x, y);
    gfx->print(speedText);

    // =================================================
    // KM/H
    // =================================================

    gfx->setTextSize(2);

    gfx->setTextColor(CYAN);

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

    gfx->fillRoundRect(
        35,
        145,
        250,
        8,
        4,
        GRAY
    );

    int bar = map(
        (int)displaySpeed,
        0,
        120,
        0,
        250
    );

    uint16_t color;

    if (displaySpeed < 70)
        color = CYAN;

    else if (displaySpeed < 100)
        color = YELLOW;

    else
        color = RED;

    gfx->fillRoundRect(
        35,
        145,
        bar,
        8,
        4,
        color
    );

    gfx->drawRoundRect(
        35,
        145,
        250,
        8,
        4,
        DARK
    );
}

// =====================================================
// STATIC UI
// =====================================================

void drawStaticUI()
{
    gfx->fillScreen(BLACK);

    drawPanel();

    drawStatus();

    drawScale();

    drawArcBackground();

    gfx->fillRoundRect(
        35,
        145,
        250,
        8,
        4,
        GRAY
    );

    gfx->drawRoundRect(
        35,
        145,
        250,
        8,
        4,
        DARK
    );
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

    // =================================================
    // BACKLIGHT PWM
    // =================================================

    pinMode(TFT_BL, OUTPUT);

    ledcAttach(TFT_BL, 5000, 8);

    // 0 - 255
    // 90 = premium brightness

    ledcWrite(TFT_BL, 40);

    // =================================================
    // DISPLAY
    // =================================================

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
    // Smooth animation

    displaySpeed +=
        (targetSpeed - displaySpeed) * 0.18;

    drawDashboard();

    // Demo animation

    if (upDir)
    {
        targetSpeed += 1;

        if (targetSpeed >= 120)
        {
            upDir = false;
        }
    }
    else
    {
        targetSpeed -= 1;

        if (targetSpeed <= 0)
        {
            upDir = true;
        }
    }

    // ~60 FPS

    delay(16);
}