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

    // ================= BACKLIGHT =================

    ledcAttach(TFT_BL, 5000, 8);
    ledcWrite(TFT_BL, 35);

    // ================= DISPLAY =================

    gfx->begin();

    gfx->invertDisplay(false);

    drawStaticUI();

    // ================= INPUT =================

    pinMode(STEER_PIN, INPUT);
    pinMode(THROTTLE_PIN, INPUT);
}

void loop()
{
    // ================= READ PWM =================

    int steerPWM = pulseIn(STEER_PIN, HIGH, 25000);
    int throttlePWM = pulseIn(THROTTLE_PIN, HIGH, 25000);

    // ================= FAILSAFE =================

    if (steerPWM == 0)
        steerPWM = 1500;

    if (throttlePWM == 0)
        throttlePWM = 1000;

    // ================= DEBUG =================

    Serial.print("STEER: ");
    Serial.print(steerPWM);

    Serial.print(" | THR: ");
    Serial.println(throttlePWM);

    // ================= MAP SPEED =================

    targetSpeed = map(throttlePWM, 1000, 2000, 0, 120);

    targetSpeed = constrain(targetSpeed, 0, 120);

    // ================= SMOOTH SPEED =================

    if (speedValue < targetSpeed)
        speedValue++;

    if (speedValue > targetSpeed)
        speedValue--;

    // ================= BLINK TIMER =================

    if (millis() - blinkTimer > 350)
    {
        blinkTimer = millis();
        blinkState = !blinkState;
    }

    // ================= CLEAR DYNAMIC AREA =================

    gfx->fillRect(20, 70, 135, 120, 0x0000);

    // ================= SPEED =================

    gfx->setTextColor(0xFFFF);
    gfx->setTextSize(5);

    gfx->setCursor(28, 85);

    if (speedValue < 10)
        gfx->print("00");
    else if (speedValue < 100)
        gfx->print("0");

    gfx->print(speedValue);

    // ================= KM/H =================

    gfx->setTextColor(0x07FF);
    gfx->setTextSize(2);

    gfx->setCursor(55, 150);
    gfx->println("KM/H");

    // ================= LEFT SIGNAL =================

    if (steerPWM < 1400)
    {
        if (blinkState)
        {
            gfx->fillTriangle(
                25, 160,
                55, 145,
                55, 175,
                0xF800
            );
        }
    }

    // ================= RIGHT SIGNAL =================

    else if (steerPWM > 1600)
    {
        if (blinkState)
        {
            gfx->fillTriangle(
                147, 160,
                117, 145,
                117, 175,
                0x001F
            );
        }
    }

    // ================= RPM BAR =================

    gfx->drawRect(10, 280, 152, 15, 0xFFFF);

    int bar = map(speedValue, 0, 120, 0, 148);

    // Clear old bar
    gfx->fillRect(12, 282, 148, 11, 0x2104);

    // Draw new bar
    gfx->fillRect(12, 282, bar, 11, 0xF800);

    delay(15);
}