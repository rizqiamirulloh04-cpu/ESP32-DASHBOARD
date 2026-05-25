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
#define UI_WHITE     0xE71C
#define SPEED_WHITE  0xFFFF
#define DARK         0x4208
#define CYAN         0x867D
#define GREEN        0x07E0
#define YELLOW       0xFFE0

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
    35,
    0
);

// ====================== VARIABLES ======================

int speedValue = 0;
int rpmBar = 0;

bool leftSignal = false;
bool rightSignal = true;

unsigned long signalMillis = 0;
bool blinkState = false;

// ====================== BATTERY ======================

void drawBattery(int percent)
{
    int x = 12;
    int y = 10;

    gfx->fillRect(0, 0, 60, 22, BLACK);

    // body
    gfx->drawRoundRect(x, y, 22, 10, 2, UI_WHITE);

    // terminal
    gfx->fillRect(x + 22, y + 3, 2, 4, UI_WHITE);

    // fill
    int fill = map(percent, 0, 100, 0, 18);

    gfx->fillRoundRect(
        x + 2,
        y + 2,
        fill,
        6,
        2,
        GREEN
    );

    // text
    gfx->setTextSize(1);
    gfx->setTextColor(UI_WHITE);

    gfx->setCursor(38, 12);
    gfx->print(percent);
    gfx->print("%");
}

// ====================== SIGNALS ======================

void drawSignals(bool leftOn, bool rightOn)
{
    gfx->fillRect(0, 0, 170, 18, BLACK);

    gfx->setTextSize(2);

    // LEFT
    gfx->setCursor(8, 6);

    if (leftOn)
        gfx->setTextColor(YELLOW);
    else
        gfx->setTextColor(DARK);

    gfx->print("<<<");

    // RIGHT
    gfx->setCursor(136, 6);

    if (rightOn)
        gfx->setTextColor(YELLOW);
    else
        gfx->setTextColor(DARK);

    gfx->print(">>>");
}

// ====================== STATIC UI ======================

void drawStaticUI()
{
    gfx->fillScreen(0x0000);
    delay(50);

    // top line
    gfx->drawFastHLine(48, 16, 74, DARK);

    // center box
    gfx->drawRoundRect(
        34,
        34,
        102,
        52,
        8,
        DARK
    );

    // rpm background
    gfx->drawRoundRect(
        18,
        112,
        134,
        6,
        3,
        DARK
    );
}

// ====================== SPEED ======================

void drawSpeed(int speed)
{
    gfx->fillRect(48, 48, 70, 26, BLACK);

    gfx->setTextColor(SPEED_WHITE);
    gfx->setTextSize(4);

    gfx->setCursor(52, 50);

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
    gfx->fillRoundRect(
        18,
        112,
        134,
        6,
        3,
        BLACK
    );

    gfx->fillRoundRect(
        18,
        112,
        value,
        6,
        3,
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

    gfx->setRotation(3);

    drawStaticUI();

    drawBattery(82);

    drawSignals(false, false);

    drawSpeed(0);

    drawRPM(0);
}

// ====================== LOOP ======================

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

    // signal blink
    if (millis() - signalMillis > 400)
    {
        signalMillis = millis();

        blinkState = !blinkState;

        drawSignals(blinkState, blinkState);
    }

    delay(40);
}