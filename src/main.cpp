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
#define UI_WHITE     0xE71C
#define SPEED_WHITE  0xFFFF
#define DARK         0x3186
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
    1,
    true,
    170,
    320
);

// ====================== VAR ======================

int speedValue = 0;
int rpmBar = 0;

unsigned long signalMillis = 0;
bool blinkState = false;

// ====================== BATTERY ======================

void drawBattery(int percent)
{
    int x = 8;
    int y = 6;

    gfx->fillRect(0, 0, 70, 20, BLACK);

    gfx->drawRoundRect(x, y, 24, 11, 2, UI_WHITE);

    gfx->fillRect(x + 24, y + 3, 2, 5, UI_WHITE);

    int fill = map(percent, 0, 100, 0, 20);

    gfx->fillRoundRect(
        x + 2,
        y + 2,
        fill,
        7,
        2,
        GREEN
    );

    gfx->setTextSize(1);
    gfx->setTextColor(UI_WHITE);

    gfx->setCursor(38, 8);
    gfx->print(percent);
    gfx->print("%");
}

// ====================== SIGNAL ======================

void drawSignals(bool on)
{
    gfx->setTextSize(2);

    // kiri
    gfx->setCursor(6, 2);

    if (on)
        gfx->setTextColor(YELLOW);
    else
        gfx->setTextColor(DARK);

    gfx->print("<<<");

    // kanan
    gfx->setCursor(128, 2);

    if (on)
        gfx->setTextColor(YELLOW);
    else
        gfx->setTextColor(DARK);

    gfx->print(">>>");
}

// ====================== STATIC UI ======================

void drawStaticUI()
{
    gfx->fillScreen(BLACK);
    delay(50);

    // garis atas
    gfx->drawFastHLine(48, 22, 74, DARK);

    // panel tengah
    gfx->drawRoundRect(
        34,
        30,
        102,
        56,
        8,
        DARK
    );

    // progress bg
    gfx->drawRoundRect(
        18,
        104,
        134,
        8,
        4,
        DARK
    );
}

// ====================== SPEED ======================

void drawSpeed(int speed)
{
    gfx->fillRect(48, 44, 72, 34, BLACK);

    gfx->setTextColor(SPEED_WHITE);
    gfx->setTextSize(4);

    gfx->setCursor(50, 44);

    if (speed < 10) gfx->print("0");
    if (speed < 100) gfx->print("0");

    gfx->print(speed);

    gfx->setTextSize(2);
    gfx->setTextColor(UI_WHITE);

    gfx->setCursor(58, 68);
    gfx->print("KM/H");
}

// ====================== RPM ======================

void drawRPM(int value)
{
    gfx->fillRoundRect(
        20,
        106,
        130,
        4,
        2,
        BLACK
    );

    gfx->fillRoundRect(
        20,
        106,
        value,
        4,
        2,
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

    rpmBar = map(speedValue, 0, 180, 0, 130);

    drawRPM(rpmBar);

    if (millis() - signalMillis > 400)
    {
        signalMillis = millis();

        blinkState = !blinkState;

        drawSignals(blinkState);
    }

    delay(40);
}