#include <Arduino.h>
#include <Arduino_GFX_Library.h>

#define TFT_BL   22

#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS   14
#define TFT_DC   15
#define TFT_RST  21

// ===== RECEIVER =====

#define STEER_PIN    1
#define THROTTLE_PIN 2

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
    false,
    172,
    320,
    34,
    0,
    34,
    0
);

int speedValue = 0;

void setup()
{
    Serial.begin(115200);

    // ===== BACKLIGHT =====

    ledcAttach(TFT_BL, 5000, 8);
    ledcWrite(TFT_BL, 35);

    // ===== DISPLAY =====

    gfx->begin();

    gfx->fillScreen(0x0000);

    // ===== RECEIVER INPUT =====

    pinMode(STEER_PIN, INPUT);
    pinMode(THROTTLE_PIN, INPUT);
}

void loop()
{
    // ===== READ PWM =====

    int steerPWM = pulseIn(STEER_PIN, HIGH, 25000);
    int throttlePWM = pulseIn(THROTTLE_PIN, HIGH, 25000);

    // ===== DEBUG SERIAL =====

    Serial.print("STEER: ");
    Serial.print(steerPWM);

    Serial.print(" | THR: ");
    Serial.println(throttlePWM);

    // ===== MAP SPEED =====

    speedValue = map(throttlePWM, 1000, 2000, 0, 120);

    if (speedValue < 0)
        speedValue = 0;

    if (speedValue > 120)
        speedValue = 120;

    // ===== CLEAR DYNAMIC AREA =====

    gfx->fillRect(0, 0, 172, 320, 0x0000);

    // ===== SPEED =====

    gfx->setTextColor(0xFFFF);
    gfx->setTextSize(5);

    gfx->setCursor(35, 80);

    if (speedValue < 10)
    {
        gfx->print("00");
    }
    else if (speedValue < 100)
    {
        gfx->print("0");
    }

    gfx->print(speedValue);

    // ===== KM/H =====

    gfx->setTextColor(0xFFFF);
    gfx->setTextSize(2);

    gfx->setCursor(55, 145);
    gfx->println("KM/H");

    // ===== LEFT / RIGHT INDICATOR =====

    if (steerPWM < 1400)
    {
        // LEFT
        gfx->fillTriangle(
            15, 160,
            45, 145,
            45, 175,
            0xF800
        );
    }
    else if (steerPWM > 1600)
    {
        // RIGHT
        gfx->fillTriangle(
            157, 160,
            127, 145,
            127, 175,
            0x001F
        );
    }
    else
    {
        // CENTER
        gfx->fillTriangle(
            86, 20,
            70, 50,
            102, 50,
            0x07E0
        );
    }

    // ===== RPM BAR =====

    gfx->drawRect(10, 280, 152, 15, 0xFFFF);

    int bar = map(speedValue, 0, 120, 0, 148);

    gfx->fillRect(12, 282, bar, 11, 0xF800);

    delay(20);
}