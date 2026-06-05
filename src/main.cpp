#include "ui_classic.h"
#include <Preferences.h>

// Definisi pin dan hardware sama seperti sebelumnya...
Arduino_DataBus *bus = new Arduino_ESP32SPI(15, 14, 7, 6, -1);
Arduino_GFX *gfx = new Arduino_ST7789(bus, 21, 1, true, 172, 320, 34, 0, 34, 0);

Preferences prefs;
int topSpeed = 0;

void setup() {
    Serial.begin(115200);
    gfx->begin();
    init_ui(gfx);
    
    prefs.begin("speedo", false);
    topSpeed = prefs.getInt("topSpeed", 0);
}

void loop() {
    // 1. Baca sensor (PWM/Analog)
    // 2. Hitung speedValue dan rpm
    // 3. Update topSpeed ke NVS
    
    // 4. Panggil UI
    static unsigned long blinkTimer = 0;
    static bool blinkState = false;
    if (millis() - blinkTimer > 350) { blinkTimer = millis(); blinkState = !blinkState; }

    draw_ui_classic(speedValue, rpmBarWidth, batteryPercent, signalDbm, currentSteerState, blinkState);
    
    delay(10);
}
