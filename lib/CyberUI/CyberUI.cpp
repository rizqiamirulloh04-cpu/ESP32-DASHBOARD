#include <Arduino.h>
#include <Arduino_GFX_Library.h>

extern Arduino_GFX *gfx;

// ======================================================
// RGB565 COLORS
// ======================================================

#define COLOR_BLACK     0x0000
#define COLOR_WHITE     0xFFFF
#define COLOR_CYAN      0x07FF
#define COLOR_GREEN     0x07E0
#define COLOR_DARKGREY  0x7BEF

// ======================================================
// INIT UI
// ======================================================

void initCyberUI()
{
    gfx->fillScreen(COLOR_BLACK);

    // HEADER

    gfx->fillRect(
        0,
        0,
        320,
        30,
        COLOR_CYAN
    );

    gfx->setTextColor(COLOR_BLACK);

    gfx->setTextSize(2);

    gfx->setCursor(10, 8);

    gfx->println("CYBER DASHBOARD");

    // BODY

    gfx->setTextColor(COLOR_WHITE);

    gfx->setCursor(20, 60);
    gfx->println("SYSTEM ONLINE");

    gfx->setCursor(20, 100);
    gfx->println("WIFI: ACTIVE");

    gfx->setCursor(20, 140);
    gfx->println("OTA: READY");

    // STATUS BOX

    gfx->drawRect(
        15,
        180,
        290,
        80,
        COLOR_CYAN
    );

    gfx->setCursor(30, 210);

    gfx->println("Awaiting telemetry...");
}

// ======================================================
// UPDATE UI
// ======================================================

void updateCyberUI()
{
    static uint32_t last = 0;

    if(millis() - last > 1000)
    {
        last = millis();

        static bool blink = false;

        blink = !blink;

        if(blink)
        {
            gfx->fillCircle(
                280,
                15,
                6,
                COLOR_GREEN
            );
        }
        else
        {
            gfx->fillCircle(
                280,
                15,
                6,
                COLOR_DARKGREY
            );
        }
    }
}