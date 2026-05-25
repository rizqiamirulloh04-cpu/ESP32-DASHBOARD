#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// ================= DISPLAY PIN =================

#define TFT_BL   22

#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS   14
#define TFT_DC   15
#define TFT_RST  21

// ================= RECEIVER =================

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
#define ORANGE  0xFD20
#define GRAY    0x8410

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
    gfx->fillScreen(BLACK);

    // TOP LINE
    gfx->drawFastHLine(0, 28, 172, CYAN);

    // RPM FRAME
    gfx->drawRect(10, 280, 152, 18, WHITE);

    // LEFT BORDER
    gfx->drawFastVLine(0, 0, 320, RED);

    // RIGHT BORDER
    gfx->drawFastVLine(171, 0, 320, BLUE);

    // CENTER ARROW
    gfx->fillTriangle(
        86, 18,
        72, 42,
        100, 42,
        GREEN
    );
}

// ================= SETUP =================

void setup()
{
    Serial.begin(115200);

    // BACKLIGHT

    ledcAttach(TFT_BL, 5000, 8);
    ledcWrite(TFT_BL, 40);

    // DISPLAY

    gfx->begin();
    gfx->invertDisplay(false);

    drawStaticUI();

    // INPUT

    pinMode(STEER_PIN, INPUT);
    pinMode(THROTTLE_PIN, INPUT);
}

// ================= LOOP =================

void loop()
{
    // READ PWM

    int steerPWM = pulseIn(STEER_PIN, HIGH, 25000);
    int throttlePWM = pulseIn(THROTTLE_PIN, HIGH, 25000);

    // FAILSAFE

    if (steerPWM == 0)
        steerPWM = 1500;

    if (throttlePWM == 0)
        throttlePWM = 1000;

    // MAP SPEED

    targetSpeed = map(throttlePWM, 1000, 2000, 0, 120);
    targetSpeed = constrain(targetSpeed, 0, 120);

    // SMOOTHING

    if (speedValue < targetSpeed)
        speedValue++;

    if (speedValue > targetSpeed)
        speedValue--;

    // BLINK

    if (millis() - blinkTimer > 300)
    {
        blinkTimer = millis();
        blinkState = !blinkState;
    }

    // CLEAR SPEED AREA

    gfx->fillRect(15, 50, 142, 120, BLACK);

    // SPEED NUMBER

    gfx->setTextColor(WHITE);
    gfx->setTextSize(5);

    gfx->setCursor(28, 70);

    if (speedValue < 10)
        gfx->print("00");
    else if (speedValue < 100)
        gfx->print("0");

    gfx->print(speedValue);

    // KMH

    gfx->setTextColor(CYAN);
    gfx->setTextSize(2);

    gfx->setCursor(56, 135);
    gfx->print("KM/H");

    // LEFT SIGNAL

    if (steerPWM < 1400)
    {
        if (blinkState)
        {
            gfx->fillTriangle(
                18, 165,
                48, 150,
                48, 180,
                ORANGE
            );
        }
    }

    // RIGHT SIGNAL

    else if (steerPWM > 1600)
    {
        if (blinkState)
        {
            gfx->fillTriangle(
                154, 165,
                124, 150,
                124, 180,
                BLUE
            );
        }
    }

    // RPM BAR

    int rpmBar = map(speedValue, 0, 120, 0, 148);

    gfx->fillRect(12, 282, 148, 14, GRAY);

    if (speedValue < 50)
    {
        gfx->fillRect(12, 282, rpmBar, 14, GREEN);
    }
    else if (speedValue < 90)
    {
        gfx->fillRect(12, 282, rpmBar, 14, YELLOW);
    }
    else
    {
        gfx->fillRect(12, 282, rpmBar, 14, RED);
    }

    delay(15);
}