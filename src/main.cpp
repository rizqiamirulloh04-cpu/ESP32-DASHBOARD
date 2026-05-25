#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// ======================================================
// WAVESHARE ESP32-C6 1.47" RACING DASHBOARD
// ======================================================

// ================= DISPLAY PINS =================

#define TFT_BL   22

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
#define YELLOW  0xFFE0
#define CYAN    0x07FF
#define DARK    0x2104

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

bool blinkState = false;
unsigned long blinkTimer = 0;

// ======================================================
// STATIC UI
// ======================================================

void drawStaticUI()
{
    gfx->fillScreen(BLACK);

    // ===== TOP CYAN LINE =====

    gfx->drawFastHLine(34, 26, 104, CYAN);

    // ===== LEFT RED BAR =====

    gfx->fillRect(18, 48, 4, 120, RED);

    // ===== RIGHT BLUE BAR =====

    gfx->fillRect(150, 48, 4, 120, BLUE);

    // ===== TOP CENTER INDICATOR =====

    gfx->fillTriangle(
        86, 12,
        74, 32,
        98, 32,
        GREEN
    );

    // ===== RPM FRAME =====

    gfx->drawRect(20, 240, 132, 16, WHITE);

    // ===== LOWER LINE =====

    gfx->drawFastHLine(20, 225, 132, DARK);
}

// ======================================================
// SETUP
// ======================================================

void setup()
{
    Serial.begin(115200);

    // ===== BACKLIGHT =====

    ledcAttach(TFT_BL, 5000, 8);
    ledcWrite(TFT_BL, 40);

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
    // ===== READ PWM =====

    int steerPWM = pulseIn(STEER_PIN, HIGH, 25000);
    int throttlePWM = pulseIn(THROTTLE_PIN, HIGH, 25000);

    // ===== FAILSAFE =====

    if (steerPWM == 0)
        steerPWM = 1500;

    if (throttlePWM == 0)
        throttlePWM = 1000;

    // ===== MAP SPEED =====

    targetSpeed = map(throttlePWM, 1000, 2000, 0, 120);

    targetSpeed = constrain(targetSpeed, 0, 120);

    // ===== SMOOTH SPEED =====

    if (speedValue < targetSpeed)
        speedValue++;

    if (speedValue > targetSpeed)
        speedValue--;

    // ===== BLINK =====

    if (millis() - blinkTimer > 350)
    {
        blinkTimer = millis();
        blinkState = !blinkState;
    }

    // ==================================================
    // CLEAR CENTER AREA
    // ==================================================

    gfx->fillRect(28, 58, 116, 110, BLACK);

// ==================================================
// SPEED NUMBER
// ==================================================

gfx->setTextColor(WHITE);
gfx->setTextSize(4);

// posisi lebih tengah
gfx->setCursor(42, 78);

// format 3 digit
if (speedValue < 10)
    gfx->print("00");
else if (speedValue < 100)
    gfx->print("0");

gfx->print(speedValue);

// ==================================================
// KM/H TEXT
// ==================================================

gfx->setTextColor(CYAN);
gfx->setTextSize(2);

gfx->setCursor(56, 128);
gfx->print("KM/H");

// ==================================================
// DIGITAL GLOW LINE
// ==================================================

gfx->drawFastHLine(45, 118, 70, DARK);

// ==================================================
// LEFT SIGNAL
// ==================================================

gfx->fillRect(22, 92, 20, 20, BLACK);

if (steerPWM < 1400 && blinkState)
{
    gfx->fillTriangle(
        22, 102,
        40, 92,
        40, 112,
        YELLOW
        );
    }

    // ==================================================
    // RIGHT SIGNAL
    // ==================================================

    gfx->fillRect(128, 95, 20, 20, BLACK);

    if (steerPWM > 1600 && blinkState)
    {
        gfx->fillTriangle(
            148, 105,
            130, 95,
            130, 115,
            YELLOW
        );
    }

    // ==================================================
    // RPM BAR
    // ==================================================

    int rpmBar = map(speedValue, 0, 120, 0, 128);

    // Clear old bar

    gfx->fillRect(22, 242, 128, 12, DARK);

    // Dynamic color

    if (speedValue < 50)
    {
        gfx->fillRect(22, 242, rpmBar, 12, GREEN);
    }
    else if (speedValue < 90)
    {
        gfx->fillRect(22, 242, rpmBar, 12, YELLOW);
    }
    else
    {
        gfx->fillRect(22, 242, rpmBar, 12, RED);
    }

    delay(15);
}