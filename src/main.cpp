#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// ======================================================
// DISPLAY PINS
// ======================================================

#define TFT_BL   22

#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS   14
#define TFT_DC   15
#define TFT_RST  21

// ======================================================
// RECEIVER INPUT
// ======================================================

#define STEER_PIN     1
#define THROTTLE_PIN  2

// ======================================================
// DISPLAY CONFIG
// ======================================================

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

// ======================================================
// VARIABLES
// ======================================================

int speedValue = 0;
int targetSpeed = 0;

bool blinkState = false;
unsigned long blinkTimer = 0;

// ======================================================
// DRAW STATIC UI
// ======================================================

void drawStaticUI()
{
    gfx->fillScreen(BLACK);

    // ===== OUTER FRAME =====

    gfx->drawRect(0, 0, 320, 172, 0x39E7);

    // ===== TOP BAR =====

    gfx->fillRect(0, 0, 320, 18, 0x18C3);

    // ===== RPM FRAME =====

    gfx->drawRect(18, 138, 284, 18, WHITE);

    // ===== CENTER MARK =====

    gfx->fillTriangle(
        160, 28,
        148, 48,
        172, 48,
        GREEN
    );

    // ===== BATTERY ICON =====

    gfx->drawRect(270, 3, 32, 12, WHITE);
    gfx->fillRect(303, 6, 3, 6, WHITE);

    gfx->fillRect(272, 5, 26, 8, GREEN);
}

// ======================================================
// SETUP
// ======================================================

void setup()
{
    Serial.begin(115200);

    // ===== BACKLIGHT =====

    ledcAttach(TFT_BL, 5000, 8);
    ledcWrite(TFT_BL, 45);

    // ===== DISPLAY =====

    gfx->begin();
    gfx->invertDisplay(false);

    drawStaticUI();

    // ===== INPUT =====

    pinMode(STEER_PIN, INPUT);
    pinMode(THROTTLE_PIN, INPUT);
}

// ======================================================
// LOOP
// ======================================================

void loop()
{
    // ==================================================
    // READ RECEIVER
    // ==================================================

    int steerPWM = pulseIn(STEER_PIN, HIGH, 25000);
    int throttlePWM = pulseIn(THROTTLE_PIN, HIGH, 25000);

    // FAILSAFE

    if (steerPWM == 0)
        steerPWM = 1500;

    if (throttlePWM == 0)
        throttlePWM = 1000;

    // ==================================================
    // MAP SPEED
    // ==================================================

    targetSpeed = map(throttlePWM, 1000, 2000, 0, 120);

    targetSpeed = constrain(targetSpeed, 0, 120);

    // ==================================================
    // SMOOTHING
    // ==================================================

    if (speedValue < targetSpeed)
        speedValue++;

    if (speedValue > targetSpeed)
        speedValue--;

    // ==================================================
    // BLINKER TIMER
    // ==================================================

    if (millis() - blinkTimer > 300)
    {
        blinkTimer = millis();
        blinkState = !blinkState;
    }

    // ==================================================
    // CLEAR DYNAMIC AREA
    // ==================================================

    gfx->fillRect(0, 20, 320, 110, BLACK);

    // ==================================================
    // BIG SPEED NUMBER
    // ==================================================

    gfx->setTextColor(WHITE);
    gfx->setTextSize(7);

    gfx->setCursor(80, 52);

    if (speedValue < 10)
        gfx->print("00");
    else if (speedValue < 100)
        gfx->print("0");

    gfx->print(speedValue);

    // ==================================================
    // KM/H TEXT
    // ==================================================

    gfx->setTextColor(0xC7FF);
    gfx->setTextSize(3);

    gfx->setCursor(118, 112);
    gfx->println("KM/H");

    // ==================================================
    // LEFT SIGNAL
    // ==================================================

    if (steerPWM < 1400)
    {
        if (blinkState)
        {
            gfx->fillTriangle(
                25, 86,
                55, 66,
                55, 106,
                0xFD20
            );
        }
    }

    // ==================================================
    // RIGHT SIGNAL
    // ==================================================

    else if (steerPWM > 1600)
    {
        if (blinkState)
        {
            gfx->fillTriangle(
                295, 86,
                265, 66,
                265, 106,
                0x07FF
            );
        }
    }

    // ==================================================
    // RPM BAR BACKGROUND
    // ==================================================

    gfx->fillRect(20, 140, 280, 14, 0x2104);

    // ==================================================
    // RPM BAR VALUE
    // ==================================================

    int rpmBar = map(speedValue, 0, 120, 0, 280);

    // COLOR ZONES

    if (rpmBar < 180)
    {
        gfx->fillRect(20, 140, rpmBar, 14, GREEN);
    }
    else if (rpmBar < 240)
    {
        gfx->fillRect(20, 140, rpmBar, 14, YELLOW);
    }
    else
    {
        gfx->fillRect(20, 140, rpmBar, 14, RED);
    }

    // ==================================================
    // THROTTLE BAR SIDE
    // ==================================================

    gfx->drawRect(6, 30, 8, 90, WHITE);

    int throttleBar = map(speedValue, 0, 120, 0, 86);

    gfx->fillRect(
        7,
        121 - throttleBar,
        6,
        throttleBar,
        0xF800
    );

    // ==================================================
    // DEBUG SERIAL
    // ==================================================

    Serial.print("STEER: ");
    Serial.print(steerPWM);

    Serial.print(" | THR: ");
    Serial.print(throttlePWM);

    Serial.print(" | SPEED: ");
    Serial.println(speedValue);

    delay(15);
}