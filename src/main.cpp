// =========================================================
// SAFE CLEAR AREA (ANTI WHITE BUG ST7789)
// =========================================================

// TOP STATUS AREA
gfx->fillRect(0, 0, 170, 24, BLACK);

// RIGHT ICON AREA
gfx->fillRect(240, 28, 40, 24, BLACK);

// =========================================================
// SAFE BATTERY ICON
// =========================================================

void drawBattery(int percent)
{
    int x = 18;
    int y = 34;

    // clear area
    gfx->fillRect(0, 28, 90, 24, BLACK);

    // body
    gfx->drawRoundRect(
        x,
        y,
        26,
        12,
        3,
        UI_WHITE
    );

    // terminal
    gfx->fillRect(
        x + 26,
        y + 3,
        3,
        6,
        UI_WHITE
    );

    // battery fill
    int fill = map(percent, 0, 100, 0, 22);

    gfx->fillRoundRect(
        x + 2,
        y + 2,
        fill,
        8,
        2,
        GREEN
    );

    // percent text
    gfx->setTextSize(1);
    gfx->setTextColor(UI_WHITE);

    gfx->setCursor(38, 37);
    gfx->print(percent);
    gfx->print("%");
}

// =========================================================
// RIGHT LAMP INDICATOR
// =========================================================

void drawLamp()
{
    gfx->fillRect(240, 28, 40, 24, BLACK);

    gfx->setTextColor(YELLOW);
    gfx->setTextSize(2);

    gfx->setCursor(255, 34);
    gfx->print("*");
}

// =========================================================
// TURN SIGNAL ANIMATION
// =========================================================

void drawSignals()
{
    static int frame = 0;

    frame++;

    if (frame > 2)
    {
        frame = 0;
    }

    // clear top
    gfx->fillRect(0, 0, 170, 24, BLACK);

    gfx->setTextSize(2);

    // LEFT SIGNAL
    for (int i = 0; i < 3; i++)
    {
        if (i == frame)
            gfx->setTextColor(YELLOW);
        else
            gfx->setTextColor(DARK);

        gfx->setCursor(8 + (i * 12), 4);
        gfx->print("<");
    }

    // RIGHT SIGNAL
    for (int i = 0; i < 3; i++)
    {
        if (i == frame)
            gfx->setTextColor(YELLOW);
        else
            gfx->setTextColor(DARK);

        gfx->setCursor(250 + (i * 12), 4);
        gfx->print(">");
    }
}

// =========================================================
// BRIGHTNESS SAFE VERSION
// =========================================================

void setup()
{
    Serial.begin(115200);

    // ==============================
    // BACKLIGHT
    // ==============================

    pinMode(TFT_BL, OUTPUT);

    // SAFE VERSION
    digitalWrite(TFT_BL, HIGH);

    // OPTIONAL PWM VERSION
    // ledcAttach(TFT_BL, 5000, 8);
    // ledcWrite(TFT_BL, 90);

    // ==============================
    // DISPLAY
    // ==============================

    gfx->begin();

    gfx->setRotation(1);

    gfx->invertDisplay(false);

    drawStaticUI();
}

// =========================================================
// COLORS
// =========================================================

#define UI_WHITE     0xE71C
#define SPEED_WHITE  0xFFFF
#define DARK         0x4208
#define BLACK        0x0000
#define GREEN        0x07E0
#define YELLOW       0xFFE0

// =========================================================
// CALL INSIDE LOOP()
// =========================================================

drawSignals();
drawBattery(82);
drawLamp();