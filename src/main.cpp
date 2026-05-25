#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// ======================================================
//                     PIN CONFIG
// ======================================================

#define TFT_BL   22

#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS   14
#define TFT_DC   15
#define TFT_RST  21

// ======================================================
//                      COLORS
// ======================================================

#define BLACK        0x0000
#define UI_WHITE     0xE71C
#define SPEED_WHITE  0xFFFF
#define DARK         0x4208
#define CYAN         0x867D
#define GREEN        0x07E0
#define YELLOW       0xFFE0

// ======================================================
//                     DISPLAY
// ======================================================

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
    0,
    true,
    170,
    320,
    35,
    0
);

// ======================================================
//                    VARIABLES
// ======================================================

int speedValue = 0;
int rpmBar = 0;

unsigned long blinkMillis = 0;
bool blinkState = false;

// ======================================================
//                    BATTERY
// ======================================================

void drawBattery(int percent)
{
    int x = 10;
    int y = 8;

    gfx->fillRect(0, 0, 60, 20, BLACK);

    // body
    gfx->drawRoundRect(x, y, 24, 10, 2, UI_WHITE);

    // terminal
    gfx->fillRect(x + 24, y + 3, 2, 4, UI_WHITE);

    // fill
    int fill = map(percent, 0, 100, 0, 20);

    gfx->fillRoundRect(
        x + 2,
        y + 2,
        fill,
        6,
        2,
        GREEN
    );

    gfx->setTextColor(UI_WHITE);
    gfx->setTextSize(1);

    gfx->setCursor(40, 10);
    gfx->print(percent);
    gfx->print("%");
}

// ======================================================
//                    SIGNALS
// ======================================================

void drawSignals(bool on)
{
    gfx->setTextSize(2);

    // LEFT
    gfx->setCursor(8, 4);

    if (on)
        gfx->setTextColor(YELLOW);
    else
        gfx->setTextColor(DARK);

    gfx->print("<<<");

    // RIGHT
    gfx->setCursor(132, 4);

    if (on)
        gfx->setTextColor(YELLOW);
    else
        gfx->setTextColor(DARK);

    gfx->print(">>>");
}

// ======================================================
//                    STATIC UI
// ======================================================

void drawStaticUI()
{
    gfx->fillScreen(BLACK);

    // top line
    gfx->drawFastHLine(50, 18, 70, DARK);

    // center speed box
    gfx->drawRoundRect(
        24,
        42,
        122,
        62,
        10,
        DARK
    );

    // rpm background
    gfx->drawRoundRect(
        18,
        122,
        134,
        8,
        4,
        DARK
    );
}

// ======================================================
//                     SPEED
// ======================================================

void drawSpeed(int speed)
{
    gfx->fillRect(40, 50, 90, 36, BLACK);

    gfx->setTextColor(SPEED_WHITE);
    gfx->setTextSize(4);

    gfx->setCursor(42, 56);

    if (speed < 10) gfx->print("0");
    if (speed < 100) gfx->print("0");

    gfx->print(speed);

    gfx->setTextColor(UI_WHITE);
    gfx->setTextSize(2);

    gfx->setCursor(58, 82);
    gfx->print("KM/H");
}

// ======================================================
//                     RPM BAR
// ======================================================

void drawRPM(int value)
{
    // clear inside bar
    gfx->fillRoundRect(
        18,
        122,
        134,
        8,
        4,
        BLACK
    );

    // redraw outline
    gfx->drawRoundRect(
        18,
        122,
        134,
        8,
        4,
        DARK
    );

    // fill
    gfx->fillRoundRect(
        18,
        122,
        value,
        8,
        4,
        CYAN
    );
}

// ======================================================
//                      SETUP
// ======================================================

void setup()
{
    Serial.begin(115200);

    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    gfx->begin();

    gfx->setRotation(1);

    gfx->fillScreen(0x0000);
    delay(50);

    drawStaticUI();

    drawBattery(82);

    drawSignals(false);

    drawSpeed(0);

    drawRPM(0);
}

// ======================================================
//                       LOOP
// ======================================================

void loop()
{
    // animate speed
    speedValue++;

    if (speedValue > 180)
        speedValue = 0;

    drawSpeed(speedValue);

    // rpm animation
    rpmBar = map(speedValue, 0, 180, 0, 134);

    drawRPM(rpmBar);

    // blink animation
    if (millis() - blinkMillis > 400)
    {
        blinkMillis = millis();

        blinkState = !blinkState;

        drawSignals(blinkState);
    }

    delay(40);
}