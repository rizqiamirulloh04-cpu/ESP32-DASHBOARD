#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// ======================================================
// WAVESHARE ESP32-C6 1.47"
// FINAL RACING DASHBOARD
// ======================================================

// ================= BACKLIGHT =================

#define TFT_BL 22

// ================= TFT PINS =================

#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS   14
#define TFT_DC   15
#define TFT_RST  21

// ================= RECEIVER INPUT =================

#define STEER_PIN    1
#define THROTTLE_PIN 2

// ================= COLORS =================

#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
#define CYAN    0x07FF
#define YELLOW  0xFFE0
#define DARK    0x2104
#define ICE     0xCE79

// ======================================================
// DISPLAY
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
// STATIC UI
// ======================================================

void drawStaticUI()
{
    gfx->fillScreen(BLACK);

    // ==================================================
    // TOP LINE
    // ==================================================

    gfx->drawFastHLine(38, 26, 64, CYAN);

    // ==================================================
    // TRIANGLE
    // ==================================================

    gfx->fillTriangle(
        70, 12,
        60, 28,
        80, 28,
        GREEN
    );

    // ==================================================
    // LEFT BAR
    // ==================================================

    gfx->fillRect(20, 54, 4, 96, RED);

    // ==================================================
    // RIGHT BAR
    // ==================================================

    gfx->fillRect(122, 54, 4, 96, BLUE);
}

// ======================================================
// SETUP
// ======================================================

void setup()
{
    Serial.begin(115200);

    // ==================================================
    // BACKLIGHT
    // ==================================================

    ledcAttach(TFT_BL, 5000, 8);

    // brightness
    ledcWrite(TFT_BL, 18);

    // ==================================================
    // DISPLAY
    // ==================================================

    gfx->begin();

    gfx->invertDisplay(false);

    drawStaticUI();

    // ==================================================
    // INPUT
    // ==================================================

    pinMode(STEER_PIN, INPUT);
    pinMode(THROTTLE_PIN, INPUT);
}

// ======================================================
// LOOP
// ======================================================

void loop()
{
    // ==================================================
    // READ PWM
    // ==================================================

    int steerPWM = pulseIn(STEER_PIN, HIGH, 25000);
    int throttlePWM = pulseIn(THROTTLE_PIN, HIGH, 25000);

    // ==================================================
    // FAILSAFE
    // ==================================================

    if (steerPWM == 0)
        steerPWM = 1500;

    if (throttlePWM == 0)
        throttlePWM = 1000;

    // ==================================================
    // SPEED MAP
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
    // BLINK TIMER
    // ==================================================

    if (millis() - blinkTimer > 350)
    {
        blinkTimer = millis();
        blinkState = !blinkState;
    }

    // ==================================================
    // CLEAR CENTER
    // ==================================================

    gfx->fillRect(28, 60, 92, 92, BLACK);

    // ==================================================
    // SPEED
    // ==================================================

    gfx->setTextColor(ICE);

    gfx->setTextSize(5);

    gfx->setCursor(24, 76);

    if (speedValue < 10)
        gfx->print("00");
    else if (speedValue < 100)
        gfx->print("0");

    gfx->print(speedValue);

    // ==================================================
    // KM/H
    // ==================================================

    gfx->setTextColor(CYAN);

    gfx->setTextSize(2);

    gfx->setCursor(44, 132);

    gfx->print("KM/H");

    // ==================================================
    // GLOW LINE
    // ==================================================

    gfx->drawFastHLine(42, 120, 58, DARK);

    // ==================================================
    // LEFT SIGNAL
    // ==================================================

    gfx->fillRect(28, 72, 14, 18, BLACK);

    if (steerPWM < 1400 && blinkState)
    {
        gfx->fillTriangle(
            28, 81,
            40, 72,
            40, 90,
            YELLOW
        );
    }

    // ==================================================
    // RIGHT SIGNAL
    // ==================================================

    gfx->fillRect(104, 72, 14, 18, BLACK);

    if (steerPWM > 1600 && blinkState)
    {
        gfx->fillTriangle(
            116, 81,
            104, 72,
            104, 90,
            YELLOW
        );
    }

    delay(15);
}