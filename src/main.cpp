// ============================================================
// MINI OEM SPEED HUD
// ESP32-C6 + ST7789 240x135
// Arduino_GFX
// ============================================================

#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <math.h>

// ============================================================
// PINS
// ============================================================

#define TFT_BL   22

#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS   14
#define TFT_DC   15
#define TFT_RST  21

// ============================================================
// DISPLAY
// ============================================================

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
    135,
    240
);

// ============================================================
// COLORS
// ============================================================

#define BLACK        0x0000
#define DARK         0x2104
#define WHITE        0xE71C
#define SPEED_WHITE  0xFFFF

#define CYAN         0x07FF
#define BLUEGRAY     0x8410

#define YELLOW       0xFFE0
#define ORANGE       0xFD20

#define GREEN        0x07E0
#define RED          0xF800

// ============================================================
// VARIABLES
// ============================================================

float displaySpeed = 0;
float targetSpeed  = 0;

int battery = 82;

unsigned long lastBlink = 0;
bool blinkState = true;

int animOffset = 0;

// ============================================================
// DRAW BATTERY
// ============================================================

void drawBattery(int x, int y, int level)
{
    // clear area
    gfx->fillRect(x, y, 52, 16, BLACK);

    // body
    gfx->drawRoundRect(x, y, 24, 12, 3, WHITE);

    // terminal
    gfx->fillRect(x + 24, y + 3, 3, 6, WHITE);

    // fill
    int fillWidth = map(level, 0, 100, 0, 20);

    uint16_t color = GREEN;

    if (level < 30)
        color = ORANGE;

    if (level < 15)
        color = RED;

    gfx->fillRoundRect(
        x + 2,
        y + 2,
        fillWidth,
        8,
        2,
        color
    );

    // text
    gfx->setTextSize(1);
    gfx->setTextColor(WHITE);

    gfx->setCursor(x + 32, y + 2);
    gfx->print(level);
    gfx->print("%");
}

// ============================================================
// DRAW SPOTLIGHT ICON
// ============================================================

void drawLampIcon(int x, int y)
{
    gfx->fillRect(x, y, 20, 16, BLACK);

    gfx->setTextColor(YELLOW);
    gfx->setTextSize(2);

    gfx->setCursor(x, y);
    gfx->print("*");
}

// ============================================================
// DRAW SIGNALS
// ============================================================

void drawSignals()
{
    if (millis() - lastBlink > 120)
    {
        lastBlink = millis();

        animOffset++;

        if (animOffset > 3)
            animOffset = 0;
    }

    // CLEAR AREA
    gfx->fillRect(0, 0, 240, 22, BLACK);

    gfx->setTextSize(2);

    // ========================================================
    // LEFT SIGNAL
    // outward animation
    // ========================================================

    for (int i = 0; i < 4; i++)
    {
        int brightness = (i == animOffset) ? YELLOW : ORANGE;

        gfx->setTextColor(brightness);

        gfx->setCursor(10 + (i * 10), 2);
        gfx->print("<");
    }

    // ========================================================
    // RIGHT SIGNAL
    // outward animation
    // ========================================================

    for (int i = 0; i < 4; i++)
    {
        int brightness = (i == animOffset) ? YELLOW : ORANGE;

        gfx->setTextColor(brightness);

        gfx->setCursor(190 + (i * 10), 2);
        gfx->print(">");
    }
}

// ============================================================
// STATIC UI
// ============================================================

void drawStaticUI()
{
    gfx->fillScreen(BLACK);

    // PANEL
    gfx->drawRoundRect(
        55,
        28,
        130,
        70,
        10,
        DARK
    );

    // SCALE
    gfx->setTextSize(1);
    gfx->setTextColor(WHITE);

    gfx->setCursor(92, 36);
    gfx->print("40");

    gfx->setCursor(154, 36);
    gfx->print("80");

    gfx->setCursor(55, 68);
    gfx->print("0");

    gfx->setCursor(194, 68);
    gfx->print("120");

    // RPM BAR BACKGROUND
    gfx->fillRoundRect(
        25,
        112,
        190,
        8,
        4,
        BLUEGRAY
    );

    // BATTERY
    drawBattery(8, 22, battery);

    // LAMP
    drawLampIcon(214, 18);
}

// ============================================================
// ACTIVE ARC
// ============================================================

void drawArc()
{
    // clear arc area
    gfx->fillRect(70, 32, 100, 40, BLACK);

    int cx = 120;
    int cy = 72;

    int radius = 38;

    // background arc
    for (int i = -150; i <= -30; i += 1)
    {
        float rad = i * 0.0174533;

        int x1 = cx + cos(rad) * radius;
        int y1 = cy + sin(rad) * radius;

        int x2 = cx + cos(rad) * (radius - 10);
        int y2 = cy + sin(rad) * (radius - 10);

        gfx->drawLine(x1, y1, x2, y2, DARK);
    }

    // active arc
    int activeAngle = map(displaySpeed, 0, 120, -150, -30);

    for (int i = -150; i <= activeAngle; i += 1)
    {
        float rad = i * 0.0174533;

        int x1 = cx + cos(rad) * radius;
        int y1 = cy + sin(rad) * radius;

        int x2 = cx + cos(rad) * (radius - 10);
        int y2 = cy + sin(rad) * (radius - 10);

        gfx->drawLine(x1, y1, x2, y2, WHITE);
    }
}

// ============================================================
// SPEED
// ============================================================

void drawSpeed()
{
    gfx->fillRect(75, 52, 90, 40, BLACK);

    char speedText[8];
    sprintf(speedText, "%03d", (int)displaySpeed);

    int x = 92;
    int y = 56;

    // shadow
    gfx->setTextColor(DARK);
    gfx->setTextSize(3);

    gfx->setCursor(x + 2, y + 2);
    gfx->print(speedText);

    // main
    gfx->setTextColor(SPEED_WHITE);

    gfx->setCursor(x, y);
    gfx->print(speedText);

    // KMH
    gfx->setTextSize(2);
    gfx->setTextColor(WHITE);

    gfx->setCursor(100, 84);
    gfx->print("KM/H");
}

// ============================================================
// RPM BAR
// ============================================================

void drawRPM()
{
    gfx->fillRoundRect(
        25,
        112,
        190,
        8,
        4,
        BLUEGRAY
    );

    int width = map(displaySpeed, 0, 120, 0, 190);

    gfx->fillRoundRect(
        25,
        112,
        width,
        8,
        4,
        CYAN
    );
}

// ============================================================
// DASHBOARD
// ============================================================

void drawDashboard()
{
    drawSignals();

    drawArc();

    drawSpeed();

    drawRPM();

    drawBattery(8, 22, battery);

    drawLampIcon(214, 18);
}

// ============================================================
// SETUP
// ============================================================

void setup()
{
    Serial.begin(115200);

    // ========================================================
    // BACKLIGHT PWM
    // ========================================================

    pinMode(TFT_BL, OUTPUT);

    ledcAttach(TFT_BL, 5000, 8);

    // brightness 0-255
    // 70-90 = premium soft
    ledcWrite(TFT_BL, 75);

    // ========================================================
    // DISPLAY
    // ========================================================

    gfx->begin();

    gfx->setRotation(1);

    gfx->invertDisplay(false);

    drawStaticUI();
}

// ============================================================
// LOOP
// ============================================================

void loop()
{
    // demo animation
    targetSpeed =
        (sin(millis() * 0.0015) + 1.0) * 60.0;

    // smoothing
    displaySpeed +=
        (targetSpeed - displaySpeed) * 0.22;

    drawDashboard();

    delay(16);
}