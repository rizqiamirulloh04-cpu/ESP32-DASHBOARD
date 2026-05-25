#include <Arduino.h>
#include <Arduino_GFX_Library.h>

#define TFT_BL   22

#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS   14
#define TFT_DC   15
#define TFT_RST  21

// ================= RECEIVER =================

#define STEER_PIN    1
#define THROTTLE_PIN 2

// ================= DISPLAY =================

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
    3,
    true,
    172,
    320,
    34,
    0,
    34,
    0
);

// ================= VARIABLES =================

int speedValue = 0;
int targetSpeed = 0;

unsigned long blinkTimer = 0;
bool blinkState = false;

// ================= STATIC UI =================

void drawStaticUI()
{
    gfx->fillScreen(0x0000);

    // RPM BAR FRAME
    gfx->drawRect(10, 280, 152, 15, 0xFFFF);

    // CENTER INDICATOR
    gfx->fillTriangle(
        86, 20,
        70, 50,
        102, 50,
        0x07E0
    );
}

void setup()
{
    Serial.begin(115200);

    // =================