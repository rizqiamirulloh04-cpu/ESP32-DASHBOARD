#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// =====================================================
// PIN CONFIG
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

#define BLACK        0x0000
#define UI_WHITE     0xE71C
#define SPEED_WHITE  0xFFFF
#define DARK         0x4208
#define CYAN         0x07FF
#define GREEN        0x07E0
#define YELLOW       0xFFE0

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
// GLOBALS
// =====================================================

int signalFrame = 0;
int speedValue = 68;

// =====================================================
// STATIC UI
// =====================================================

void drawStaticUI()
{
    gfx->fillScreen(BLACK);

    // panel tengah
    gfx->drawRoundRect(42, 34, 236, 90, 10, DARK);

    // garis bawah
    gfx->drawRoundRect(20, 138, 280, 10, 5, DARK);
}

// =====================================================
// SIGNALS
// =====================================================

void drawSignals()
{
    gfx->fillRect(0, 0, 170, 24, BLACK);

    gfx->setTextSize(2);

    // kiri
    for (int i = 0; i < 4; i++)
    {
        if (i <= signalFrame)
            gfx->setTextColor(YELLOW);
        else
            gfx->setTextColor(DARK);

        gfx->setCursor(8 + (i * 12), 4);
        gfx->print("<");
    }

    // kanan
    for (int i = 0; i < 4; i++)
    {
        if (i <= signalFrame)
            gfx->setTextColor(YELLOW);
        else
            gfx->setTextColor(DARK);

        gfx->setCursor(264 + (i * 12), 4);
        gfx->print(">");
    }

    signalFrame++;

    if (signalFrame > 3)
        signalFrame = 0;
}

// =====================================================
// BATTERY
// =====================================================

void drawBattery(int percent)
{
    int x = 10;
    int y = 28;

    gfx->fillRect(0, 28, 110, 24, BLACK);

    // body
    gfx->drawRoundRect(x, y, 26, 12, 3, UI_WHITE);

    // terminal
    gfx->fillRect(x + 26, y + 3, 3, 6, UI_WHITE);

    // fill
    int fill = map(percent, 0, 100, 0, 22);

    gfx->fillRoundRect(
        x + 2,
        y + 2,
        fill,
        8,
        2,
        GREEN
    );

    // text
    gfx->setTextColor(UI_WHITE);
    gfx->setTextSize(1);

    gfx->setCursor(42, 31);
    gfx->print(percent);
    gfx->print("%");
}

// =====================================================
// LAMP
// =====================================================

void drawLamp()
{
    gfx->fillRect(240, 28, 40, 24, BLACK);

    gfx->setTextColor(YELLOW);
    gfx->setTextSize(2);

    gfx->setCursor(266, 30);
    gfx->print("*");
}

// =====================================================
// SPEED
// =====================================================

void drawSpeed(int speed)
{
    char buf[4];
    sprintf(buf, "%03d", speed);

    // clear angka
    gfx->fillRect(95, 58, 130, 40, BLACK);

    // shadow
    gfx->setTextColor(DARK);
    gfx->setTextSize(4);

    gfx->setCursor(104, 66);
    gfx->print(buf);

    // main text
    gfx->setTextColor(SPEED_WHITE);

    gfx->setCursor(102, 64);
    gfx->print(buf);

    // KMH
    gfx->setTextColor(UI_WHITE);
    gfx->setTextSize(2);

    gfx->setCursor(126, 102);
    gfx->print("KM/H");
}

// =====================================================
// BAR
// =====================================================

void drawBar(int value)
{
    gfx->fillRoundRect(
        22,
        140,
        276,
        6,
        3,
        DARK
    );

    int width = map(value, 0, 100, 0, 276);

    gfx->fillRoundRect(
        22,
        140,
        width,
        6,
        3,
        CYAN
    );
}

// =====================================================
// SETUP
// =====================================================

void setup()
{
    Serial.begin(115200);

    // backlight aman
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

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
    drawSignals();

    drawBattery(82);

    drawLamp();

    drawSpeed(speedValue);

    drawBar(speedValue);

    speedValue++;

    if (speedValue > 120)
        speedValue = 0;

    delay(120);
}