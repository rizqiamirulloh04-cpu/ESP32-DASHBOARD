#include <Arduino.h>
#include <Arduino_GFX_Library.h>

#define TFT_BL   15

#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_DC   8
#define TFT_CS   14
#define TFT_RST  9

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
    172,
    320,
    34,
    0
);

void setup()
{
    Serial.begin(115200);

    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    // HARD RESET LCD
    pinMode(TFT_RST, OUTPUT);

    digitalWrite(TFT_RST, LOW);
    delay(100);

    digitalWrite(TFT_RST, HIGH);
    delay(200);

    gfx->begin();

    delay(200);

    gfx->fillScreen(BLACK);

    delay(1000);

    gfx->fillScreen(RED);
    delay(1000);

    gfx->fillScreen(GREEN);
    delay(1000);

    gfx->fillScreen(BLUE);
    delay(1000);

    gfx->fillScreen(WHITE);
}

void loop()
{
}