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

// soft white-blue
#define ICE     0xCE79

// ======================================================
// DISPLAY
// ======================================================

Arduino_DataBus *bus = new Arduino_ESP32SPI(
    8,   // DC
    14,  // CS
    7,   // SCK
    6,   // MOSI
    GFX_NOT_DEFINED // MISO
);

Arduino_GFX *gfx = new Arduino_ST7789(
    bus,
    9,    // RST
    1,    // rotation
    true, // IPS
    172,  // width
    320,  // height
    34,   // col offset
    0,    // row offset
    35,   // col offset2
    0     // row offset2
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

    // ===== LEFT BAR =====

    gfx->fillRect(16, 54, 4, 96, RED);

    // ===== RIGHT BAR =====

    gfx->fillRect(118, 54, 4, 96, BLUE);

    // ===== TOP LINE =====

    gfx->drawFastHLine(32, 26, 70, CYAN);

    // ===== TOP TRIANGLE =====

    gfx->fillTriangle(
        67, 12,
        57, 28,
        77, 28,
        GREEN
    );
}

// ======================================================
// SETUP
// ======================================================

void setup()
{
    Serial.begin(115200);

    // ===== BACKLIGHT =====

    ledcAttach(TFT_BL, 5000, 8);

    // brightness
    ledcWrite(TFT_BL, 18);

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

    gfx->fillRect(24, 60, 90, 80, BLACK);

    // ==================================================
    // SPEED NUMBER
    // ==================================================

    gfx->setTextColor(ICE);

    gfx->setTextSize(4);

    gfx->setCursor(30, 84);

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

    gfx->setCursor(42, 124);

    gfx->print("KM/H");

    // ==================================================
    // GLOW LINE
    // ==================================================

    gfx->drawFastHLine(42, 118, 52, DARK);

    // ==================================================
    // LEFT SIGNAL
    // ==================================================

    gfx->fillRect(24, 70, 12, 20, BLACK);

    if (steerPWM < 1400 && blinkState)
    {
        gfx->fillTriangle(
            24, 80,
            36, 72,
            36, 88,
            YELLOW
        );
    }

    // ==================================================
    // RIGHT SIGNAL
    // ==================================================

    gfx->fillRect(102, 70, 12, 20, BLACK);

    if (steerPWM > 1600 && blinkState)
    {
        gfx->fillTriangle(
            114, 80,
            102, 72,
            102, 88,
            YELLOW
        );
    }

    delay(15);
}