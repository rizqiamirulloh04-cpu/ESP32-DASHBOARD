#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// ======================================================
// WAVESHARE ESP32-C6 1.47" LCD
// FULL WORKING TEST + DASHBOARD
// ======================================================

// ---------------- BACKLIGHT ----------------
#define TFT_BL 15

// ---------------- SPI LCD ----------------
#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_DC   8
#define TFT_RST  9
#define TFT_CS   14

// ======================================================
// LCD CONFIG
// ======================================================

Arduino_DataBus *bus = new Arduino_ESP32SPI(
    TFT_DC,
    TFT_CS,
    TFT_SCLK,
    TFT_MOSI,
    GFX_NOT_DEFINED
);

// CONFIG PALING COCOK UNTUK WAVESHARE 1.47"
Arduino_GFX *gfx = new Arduino_ST7789(
    bus,
    TFT_RST,
    1,      // rotation
    true,   // IPS
    320,    // width
    172,    // height
    35,     // col offset
    0       // row offset
);

// ======================================================
// DRAW UI
// ======================================================

void drawDashboard(int speed)
{
    gfx->fillScreen(BLACK);

    // BORDER
    gfx->drawRoundRect(0, 0, 320, 172, 12, DARKGREY);

    // LEFT BAR
    gfx->fillRoundRect(18, 28, 12, 110, 8, RED);

    // RIGHT BAR
    gfx->fillRoundRect(290, 28, 12, 110, 8, BLUE);

    // TOP INDICATOR
    gfx->fillTriangle(
        150, 10,
        170, 10,
        160, 0,
        GREEN
    );

    // SPEED NUMBER
    gfx->setTextSize(6);
    gfx->setTextColor(WHITE);

    char buf[10];
    sprintf(buf, "%03d", speed);

    gfx->setCursor(90, 55);
    gfx->println(buf);

    // KM/H
    gfx->setTextSize(3);
    gfx->setTextColor(CYAN);

    gfx->setCursor(118, 115);
    gfx->println("KM/H");

    // LOWER LINE
    gfx->drawFastHLine(60, 150, 200, DARKGREY);

    // CORNER LIGHTS
    gfx->fillCircle(20, 20, 5, GREEN);
    gfx->fillCircle(300, 20, 5, RED);
}

// ======================================================
// SETUP
// ======================================================

void setup()
{
    Serial.begin(115200);

    // BACKLIGHT
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    // LCD INIT
    gfx->begin();

    // OPTIONAL
    gfx->setRotation(1);

    // TEST COLORS
    gfx->fillScreen(RED);
    delay(500);

    gfx->fillScreen(GREEN);
    delay(500);

    gfx->fillScreen(BLUE);
    delay(500);

    gfx->fillScreen(BLACK);

    drawDashboard(0);
}

// ======================================================
// LOOP
// ======================================================

void loop()
{
    static int speed = 0;
    static bool up = true;

    drawDashboard(speed);

    if (up)
    {
        speed++;

        if (speed >= 180)
        {
            up = false;
        }
    }
    else
    {
        speed--;

        if (speed <= 0)
        {
            up = true;
        }
    }

    delay(30);
}