#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// ====================== PIN ======================

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

// ====================== VAR ======================

int speedValue = 0;
int rpmBar = 0;

unsigned long signalMillis = 0;
bool blinkState = false;

// ====================== BATTERY ======================

void drawBattery(int percent)
{
    int x = 10;
    int y = 10;

    gfx->fillRect(0, 0, 60, 20, BLACK);

    gfx->drawRoundRect(x, y, 22, 10, 2, UI_WHITE);

    gfx->fillRect(x + 22, y + 3, 2, 4, UI_WHITE);

    int fill = map(percent, 0, 100, 0, 18);

    gfx->fillRoundRect(
        x + 2,
        y + 2,
        fill,
        6,
        2,
        GREEN
    );

    gfx->setTextSize(1);
    gfx->setTextColor(UI_WHITE);

    gfx->setCursor(38, 12);
    gfx->print(percent);
    gfx->print("%");
}

// ====================== SIGNAL ======================

void drawSignals(bool on)
{
    gfx->setTextSize(2);

    // kiri
    gfx->setCursor(8, 6);

    if (on)
        gfx->setTextColor(YELLOW);
    else
        gfx->setTextColor(DARK);

    gfx->print("<<<");

    // kanan
    gfx->setCursor(136, 6);

    if (on)
        gfx->setTextColor(YELLOW);
    else
        gfx->setTextColor(DARK);

    gfx->print(">>>");
}

// ====================== STATIC ======================

void drawStaticUI()
{
    gfx->fillScreen(BLACK);
    delay(50);

    // garis atas
    gfx->drawFastHLine(48, 22, 74, DARK);

    // panel speed
    gfx->drawRoundRect(
        34,
        34,
        102,
        52,
        8,
        DARK
    );

    // rpm bg
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
    gfx->fillRect(48, 48, 72, 30, BLACK);

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

// ====================== RPM ======================

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

    drawSignals(false);

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

    rpmBar = map(speedValue, 0, 180, 0, 134);

    drawRPM(rpmBar);

    if (millis() - signalMillis > 400)
    {
        signalMillis = millis();

        blinkState = !blinkState;

        drawSignals(blinkState);
    }

    delay(40);
}