#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// =====================
// WAVESHARE ESP32-C6 LCD 1.47"
// ST7789 172x320
// =====================

// ===== TFT PIN =====
#define TFT_BL   15

#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_DC   8
#define TFT_RST  9
#define TFT_CS   14

// =====================
// COLOR
// =====================
#define BLACK   0x0000
#define WHITE   0xFFFF
#define CYAN    0x07FF
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define YELLOW  0xFFE0
#define GRAY    0x8410

// =====================
// DISPLAY
// =====================
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
    0,      // rotation
    true,   // IPS
    172,    // width
    320,    // height
    34,     // col offset
    0       // row offset
);

// =====================
// GLOBAL
// =====================
int speedValue = 0;
int battery = 87;
int rssi = -52;

// =====================
// DRAW BACKGROUND
// =====================
void drawStaticUI()
{
    gfx->fillScreen(BLACK);

    // ===== TOP CYAN LINE =====
    gfx->drawFastHLine(70, 18, 180, CYAN);

    // ===== TRIANGLE =====
    gfx->fillTriangle(
        155, 6,
        145, 18,
        165, 18,
        GREEN
    );

    // ===== LEFT BAR =====
    gfx->fillRoundRect(
        45,
        48,
        6,
        150,
        3,
        RED
    );

    // ===== RIGHT BAR =====
    gfx->fillRoundRect(
        269,
        48,
        6,
        150,
        3,
        CYAN
    );

    // ===== TITLE =====
    gfx->setTextColor(CYAN);
    gfx->setTextSize(2);

    gfx->setCursor(118, 35);
    gfx->print("SPEED");

    // ===== BOTTOM LINE =====
    gfx->drawFastHLine(20, 235, 280, GRAY);

    // ===== INFO =====
    gfx->setTextColor(WHITE);
    gfx->setTextSize(2);

    gfx->setCursor(20, 255);
    gfx->print("BAT");

    gfx->setCursor(120, 255);
    gfx->print("RSSI");

    gfx->setCursor(230, 255);
    gfx->print("TEMP");
}

// =====================
// DRAW SPEED
// =====================
void drawSpeed(int value)
{
    // clear old area
    gfx->fillRect(70, 70, 180, 120, BLACK);

    char buf[10];
    sprintf(buf, "%03d", value);

    // ===== SPEED NUMBER =====
    gfx->setTextColor(WHITE);
    gfx->setTextSize(6);

    int16_t x1, y1;
    uint16_t w, h;

    gfx->getTextBounds(buf, 0, 0, &x1, &y1, &w, &h);

    int x = (320 - w) / 2;
    int y = 110;

    gfx->setCursor(x, y);
    gfx->print(buf);

    // ===== KM/H =====
    gfx->setTextColor(CYAN);
    gfx->setTextSize(3);

    gfx->getTextBounds("KM/H", 0, 0, &x1, &y1, &w, &h);

    x = (320 - w) / 2;

    gfx->setCursor(x, 165);
    gfx->print("KM/H");
}

// =====================
// DRAW BOTTOM INFO
// =====================
void drawInfo()
{
    // clear
    gfx->fillRect(0, 270, 320, 40, BLACK);

    gfx->setTextSize(3);

    // BATTERY
    gfx->setTextColor(GREEN);
    gfx->setCursor(18, 280);
    gfx->print(battery);
    gfx->print("%");

    // RSSI
    gfx->setTextColor(CYAN);
    gfx->setCursor(110, 280);
    gfx->print(rssi);

    // TEMP
    gfx->setTextColor(YELLOW);
    gfx->setCursor(240, 280);
    gfx->print("32C");
}

// =====================
// DRAW SPEED BAR
// =====================
void drawBars(int value)
{
    int barHeight = map(value, 0, 180, 0, 150);

    // clear inside
    gfx->fillRect(52, 48, 12, 150, BLACK);
    gfx->fillRect(257, 48, 12, 150, BLACK);

    // left glow
    gfx->fillRoundRect(
        52,
        198 - barHeight,
        12,
        barHeight,
        3,
        RED
    );

    // right glow
    gfx->fillRoundRect(
        257,
        198 - barHeight,
        12,
        barHeight,
        3,
        CYAN
    );
}

// =====================
// SETUP
// =====================
void setup()
{
    Serial.begin(115200);

    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    gfx->begin();

    gfx->fillScreen(BLACK);

    drawStaticUI();
    drawSpeed(0);
    drawInfo();
}

// =====================
// LOOP
// =====================
void loop()
{
    // speed animation
    for (int i = 0; i <= 180; i++)
    {
        speedValue = i;

        drawSpeed(speedValue);
        drawBars(speedValue);

        delay(15);
    }

    for (int i = 180; i >= 0; i--)
    {
        speedValue = i;

        drawSpeed(speedValue);
        drawBars(speedValue);

        delay(15);
    }
}