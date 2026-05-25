#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// ================= DISPLAY PINS =================

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
#define GRAY    0x4208

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

// ================= STATIC UI =================

void drawStaticUI()
{
    gfx->fillScreen(BLACK);

    // TOP LINE
    gfx->drawFastHLine(10, 26, 152, CYAN);

    // SIDE BARS
    gfx->fillRect(8, 40, 4, 180, RED);
    gfx->fillRect(160, 40, 4, 180, BLUE);

    // CENTER ARROW
    gfx->fillTriangle(
        86, 14,
        72, 38,
        100, 38,
        GREEN
    );

    // RPM FRAME
    gfx->drawRect(10, 250, 152, 18, WHITE);
}

// ================= SETUP =================

void setup()
{
    Serial.begin(115200);

    // BACKLIGHT
    ledcAttach(TFT_BL, 5000, 8);
    ledcWrite(TFT_BL, 35);

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

    // DEBUG

    Serial.print("STEER: ");
    Serial.print(steerPWM);

    Serial.print(" | THR: ");
    Serial.println(throttlePWM);

    // MAP SPEED

    targetSpeed = map(throttlePWM, 1000, 2000, 0, 120);

    targetSpeed = constrain(targetSpeed, 0, 120);

    // SMOOTH SPEED

    if (speedValue < targetSpeed)
        speedValue++;

    if (speedValue > targetSpeed)
        speedValue--;

    // BLINK

    if (millis() - blinkTimer > 350)
    {
        blinkTimer = millis();
        blinkState = !blinkState;
    }

    // CLEAR CENTER AREA

    gfx->fillRect(20, 60, 132, 120, BLACK);

    // SPEED

    gfx->setTextColor(WHITE);
    gfx->setTextSize(5);

    gfx->setCursor(28, 85);

    if (speedValue < 10)
        gfx->print("00");
    else if (speedValue < 100)
        gfx->print("0");

    gfx->print(speedValue);

    // KM/H

    gfx->setTextColor(CYAN);
    gfx->setTextSize(2);

    gfx->setCursor(52, 145);
    gfx->print("KM/H");

    // LEFT SIGNAL

    if (steerPWM < 1400)
    {
        if (blinkState)
        {
            gfx->fillTriangle(
                20, 110,
                40, 95,
                40, 125,
                YELLOW
            );
        }
    }

    // RIGHT SIGNAL

    else if (steerPWM > 1600)
    {
        if (blinkState)
        {
            gfx->fillTriangle(
                152, 110,
                132, 95,
                132, 125,
                YELLOW
            );
        }
    }

    // RPM BAR

    int rpmBar = map(speedValue, 0, 120, 0, 148);

    gfx->fillRect(12, 252, 148, 14, GRAY);

    if (speedValue < 50)
    {
        gfx->fillRect(12, 252, rpmBar, 14, GREEN);
    }
    else if (speedValue < 90)
    {
        gfx->fillRect(12, 252, rpmBar, 14, YELLOW);
    }
    else
    {
        gfx->fillRect(12, 252, rpmBar, 14, RED);
    }

    delay(15);
}