#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// ====================== PIN CONFIG ======================

#define TFT_BL   22

#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS   14
#define TFT_DC   15
#define TFT_RST  21

// ====================== COLORS ======================

#define BLACK        0x0000
#define UI_WHITE     0xDEFB
#define SPEED_WHITE  0xFFFF
#define DARK         0x39E7
#define CYAN         0x867D
#define GREEN        0x07E0
#define YELLOW       0xFEE0

// ====================== DISPLAY ======================

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
    0,
    35
);

// ====================== VARIABLES ======================

int speedValue = 0;
int rpmBar = 0;

unsigned long signalMillis = 0;
bool blinkState = false;

// ====================== BATTERY ======================

void drawBattery(int percent)
{
    int x = 8;
    int y = 8;

    gfx->fillRect(0, 0, 70, 20, BLACK);

    // body
    gfx->drawRoundRect(x, y, 20, 9, 2, UI_WHITE);

    // terminal
    gfx->fillRect(x + 20, y + 3, 2, 3, UI_WHITE);

    // fill
    int fill = map(percent, 0, 100, 0, 16);

    gfx->fillRoundRect(
        x + 2,
        y + 2,
        fill,
        5,
        2,
        GREEN
    );

    gfx->setTextSize(1);
    gfx->setTextColor(UI_WHITE);

    gfx->setCursor(34, 9);
    gfx->print(percent);
    gfx->print("%");
}

// ====================== SIGNALS ======================

void drawSignals(bool on)
{
    gfx->setTextSize(2);

    // LEFT
    gfx->setCursor(6, 2);
    gfx->setTextColor(on ? YELLOW : DARK);
    gfx->print("<<<");

    // RIGHT
    gfx->setCursor(132, 2);
    gfx->setTextColor(on ? YELLOW : DARK);
    gfx->print(">>>");
}

// ====================== STATIC UI ======================

void drawStaticUI()
{
    gfx->fillScreen(BLACK);
    delay(50);

    // top line
    gfx->drawFastHLine(50, 16, 70, DARK);

    // center speed box
    gfx->drawRoundRect(
        34,
        28,
        102,
        58,
        8,
        DARK
    );

    // arc accent
    gfx->fillArc(
        85,
        48,
        24,
        16,
        200,
        340,
        CYAN
    );

    // rpm background
    gfx->drawRoundRect(
        20,
        102,
        130,
        8,
        4,
        DARK
    );
}

// ====================== SPEED ======================

void drawSpeed(int speed)
{
    gfx->fillRect(48, 46, 74, 28, BLACK);

    gfx->setTextColor(SPEED_WHITE);
    gfx->setTextSize(4);

    gfx->setCursor(50, 50);

    if (speed < 10) gfx->print("0");
    if (speed < 100) gfx->print("0");

    gfx->print(speed);

    gfx->setTextSize(2);
    gfx->setTextColor(UI_WHITE);

    gfx->setCursor(61, 72);
    gfx->print("KM/H");
}

// ====================== RPM BAR ======================

void drawRPM(int value)
{
    if (value > 130) value = 130;

    gfx->fillRoundRect(
        20,
        102,
        130,
        8,
        4,
        BLACK
    );

    gfx->fillRoundRect(
        20,
        102,
        value,
        8,
        4,
        CYAN
    );
}

// ====================== SETUP ======================

void setup()
{
    Serial.begin(115200);

    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    gfx->begin();

    gfx->setRotation(1);

    drawStaticUI();

    drawBattery(88);

    drawSignals(true);

    drawSpeed(0);

    drawRPM(0);
}

// ====================== LOOP ======================

void loop()
{
    speedValue++;

    if (speedValue > 180)
        speedValue = 0;

    drawSpeed(speedValue);

    rpmBar = map(speedValue, 0, 180, 0, 130);

    drawRPM(rpmBar);

    // blink signal
    if (millis() - signalMillis > 500)
    {
        signalMillis = millis();

        blinkState = !blinkState;

        drawSignals(blinkState);
    }

    delay(35);
}