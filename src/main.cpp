#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// ======================================================
// COLOR
// ======================================================

#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
#define CYAN    0x07FF

// ======================================================
// WAVESHARE ESP32-C6 LCD 1.47
// ======================================================

#define TFT_BL    22

#define TFT_MOSI  6
#define TFT_SCLK  7
#define TFT_DC    15
#define TFT_CS    14
#define TFT_RST   21

// ======================================================
// DISPLAY BUS
// ======================================================

Arduino_DataBus *bus = new Arduino_ESP32SPI(
    TFT_DC,
    TFT_CS,
    TFT_SCLK,
    TFT_MOSI,
    GFX_NOT_DEFINED
);

// ======================================================
// DISPLAY
// ======================================================

Arduino_GFX *gfx = new Arduino_ST7789(
    bus,
    TFT_RST,
    1,
    true,
    172,
    320,
    34,
    0,
    34,
    0
);

// ======================================================
// SETUP
// ======================================================

void setup()
{
    Serial.begin(115200);

    // BACKLIGHT

    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    // RESET

    pinMode(TFT_RST, OUTPUT);

    digitalWrite(TFT_RST, HIGH);
    delay(50);

    digitalWrite(TFT_RST, LOW);
    delay(50);

    digitalWrite(TFT_RST, HIGH);
    delay(200);

    // DISPLAY INIT

    gfx->begin();

    gfx->setRotation(1);

    gfx->fillScreen(BLACK);

    // BORDER

    gfx->drawRect(
        0,
        0,
        320,
        172,
        CYAN
    );

    // TEXT

    gfx->setTextColor(WHITE);

    gfx->setTextSize(2);

    gfx->setCursor(20, 40);
    gfx->println("DISPLAY OK");

    gfx->setCursor(20, 80);
    gfx->println("ESP32-C6");

    gfx->setCursor(20, 120);
    gfx->println("WAVESHARE LCD");

    // COLOR TEST

    gfx->fillRect(250, 20, 40, 40, RED);

    gfx->fillRect(250, 70, 40, 40, GREEN);

    gfx->fillRect(250, 120, 40, 40, BLUE);

    Serial.println("LCD TEST DONE");
}

void loop()
{
}