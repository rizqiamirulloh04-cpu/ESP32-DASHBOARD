#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

#define TFT_BL 22

void setup()
{
    Serial.begin(115200);

    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    tft.init();

    tft.setRotation(1);

    tft.fillScreen(TFT_BLACK);

    tft.setTextColor(TFT_WHITE);

    tft.setTextSize(2);

    tft.drawString("DISPLAY OK", 20, 40);

    tft.drawString("ESP32-C6", 20, 80);

    tft.fillRect(250, 20, 40, 40, TFT_RED);
    tft.fillRect(250, 70, 40, 40, TFT_GREEN);
    tft.fillRect(250, 120, 40, 40, TFT_BLUE);

    Serial.println("LCD READY");
}

void loop()
{
}