#include <Arduino.h>
#include <SPI.h>
#include <Arduino_GFX_Library.h>

#define BLACK     0x0000
#define WHITE     0xFFFF
#define RED       0xF800
#define GREEN     0x07E0
#define BLUE      0x001F
#define CYAN      0x07FF
#define YELLOW    0xFFE0
#define MAGENTA   0xF81F
#define DARKGREY  0x7BEF

// ======================
// WAVESHARE ESP32-C6 LCD 1.47
// ======================

#define TFT_BL    15

#define TFT_MOSI  6
#define TFT_SCLK  7
#define TFT_CS    14
#define TFT_DC    8
#define TFT_RST   9

// ======================
// DISPLAY
// ======================

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
    1,      // rotation
    true,   // IPS
    172,    // width
    320,    // height
    34,     // col offset 1
    0,      // row offset 1
    34,     // col offset 2
    0       // row offset 2
);

void setup()
{
    Serial.begin(115200);

    // BACKLIGHT
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    delay(200);

    // DISPLAY
    gfx->begin();

    gfx->fillScreen(BLACK);

    gfx->setTextColor(WHITE);
    gfx->setTextSize(2);

    gfx->setCursor(20, 40);
    gfx->println("LCD TEST");

    gfx->setCursor(20, 80);
    gfx->println("ESP32-C6");

    gfx->drawRect(0, 0, 320, 172, RED);

    Serial.println("DISPLAY OK");
}

void loop()
{
}