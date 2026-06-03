#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// ======================================================================
// WAVESHARE ESP32-C6 1.47" - STANDARD TEXT DASHBOARD (FIXED POSITION)
// ======================================================================

#define TFT_BL 22
#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS   14
#define TFT_DC   15
#define TFT_RST  21

#define STEER_PIN    1
#define THROTTLE_PIN 2
#define BATTERY_PIN  3  
#define SIGNAL_PIN   4  

// COLORS (Format RGB565 16-Bit)
#define BLACK          0x0000
#define WHITE          0xFFFF
#define RED            0xF800
#define GREEN          0x07E0 
#define YELLOW         0xFFE0
#define GLOW_BLUE_3    0x0012 
#define NEON_GLOW      0x03EF 

// INITIALISASI HARDWARE DISPLAY & KANVAS
Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, GFX_NOT_DEFINED);
Arduino_GFX *gfx = new Arduino_ST7789(bus, TFT_RST, 1, true, 172, 320, 34, 0, 34, 0);

// Canvas internal anti-flicker
#define CANVAS_W 140
#define CANVAS_H 60
Arduino_Canvas *canvas = new Arduino_Canvas(CANVAS_W, CANVAS_H, gfx);

int speedValue = 0; 
int lastSpeedValue = -1; 
int targetSpeed = 0;
bool blinkState = false;
unsigned long blinkTimer = 0;
int lastSteerState = 0; 

// Variabel untuk data Sensor
float batteryVolt = 0.0;
float lastBatteryVolt = -1.0;
int signalStrength = 0;
int lastSignalStrength = -1;
unsigned long sensorTimer = 0;

void drawCyberpunkGlow() {
    for (int r = 85; r > 0; r -= 3) {
        uint16_t glowColor = BLACK;
        if (r > 65)       glowColor = 0x0004;
        else if (r > 45)  glowColor = 0x000A;
        else if (r > 25)  glowColor = 0x0012;
        else              glowColor = 0x011A;
        gfx->fillCircle(160, 80, r, glowColor);
    }
}

void setup() {
    Serial.begin(115200);
    
    ledcAttach(TFT_BL, 5000, 8);
    ledcWrite(TFT_BL, 128); 

    gfx->begin();
    gfx->invertDisplay(false);
    gfx->setRotation(1); 

    canvas->begin();
    gfx->fillScreen(BLACK);
    drawCyberpunkGlow();

    gfx->setTextColor(WHITE); 
    gfx->setTextSize(2);
    gfx->setCursor(136, 125); 
    gfx->print("KM/H");

    pinMode(STEER_PIN, INPUT);
    pinMode(THROTTLE_PIN, INPUT);
    pinMode(BATTERY_PIN, INPUT);
    pinMode(SIGNAL_PIN, INPUT);
}

void loop() {
    int steerPWM = pulseIn(STEER_PIN, HIGH, 20000);
    int throttlePWM = pulseIn(THROTTLE_PIN, HIGH, 20000);

    if (steerPWM == 0)     steerPWM = 1500;
    if (throttlePWM == 0)  throttlePWM = 1500; 

    if (throttlePWM < 1490) { 
        targetSpeed = map(throttlePWM, 1500, 1000, 0, 120);
        targetSpeed = constrain(targetSpeed, 0, 120);
    } else if (throttlePWM > 1510) { 
        targetSpeed = map(throttlePWM, 1500, 2000, 0, 50);
        targetSpeed = constrain(targetSpeed, 0, 50);
    } else {
        targetSpeed = 0; 
    }

    if (speedValue < targetSpeed) {
        speedValue += 4; 
        if (speedValue > targetSpeed) speedValue = targetSpeed;
    }
    if (speedValue > targetSpeed) {
        speedValue -= 4;
        if (speedValue < targetSpeed) speedValue = targetSpeed;
    }

    bool toggleBlink = false;
    if (millis() - blinkTimer > 350) {
        blinkTimer = millis();
        blinkState = !blinkState;
        toggleBlink = true; 
    }

    int currentSteerState = 0; 
    if (steerPWM < 1400)      currentSteerState = 1;
    else if (steerPWM > 1600) currentSteerState = 2;

    if (currentSteerState != lastSteerState || toggleBlink) {
        if (!blinkState || currentSteerState == 0) {
            gfx->fillRect(15, 50, 45, 55, BLACK);  
            gfx->fillRect(260, 50, 50, 55, BLACK); 
        }
        lastSteerState = currentSteerState;
    }

    // ==================================================================
    // PEMBACAAN DAN DISPLAY INDIKATOR (BATERAI & SINYAL KE KANAN)
    // ==================================================================
    if (millis() - sensorTimer > 200) {
        sensorTimer = millis();

        int rawBat = analogRead(BATTERY_PIN);
        batteryVolt = (rawBat / 4095.0) * 3.3 * 4.0; 

        int rawSig = analogRead(SIGNAL_PIN);
        signalStrength = map(rawSig, 0, 4095, 0, 100);
        signalStrength = constrain(signalStrength, 0, 100);

        // Render Baterai Kiri Atas
        if (abs(batteryVolt - lastBatteryVolt) > 0.05) {
            gfx->fillRect(10, 15, 65, 20, BLACK); 
            gfx->setTextSize(2);
            gfx->setTextColor(GREEN); 
            gfx->setCursor(10, 15);
            gfx->printf("%.1fV", batteryVolt);
            lastBatteryVolt = batteryVolt;
        }

        // Render Sinyal Kanan Atas - SUDAH DIPOSISIKAN MEPET KANAN (X: 275)
        if (signalStrength != lastSignalStrength) {
            gfx->fillRect(265, 15, 50, 20, BLACK); // Hapus area lama pas di kanan
            gfx->setTextSize(2);
            uint16_t sigColor = (signalStrength < 40) ? RED : GREEN;
            gfx->setTextColor(sigColor);
            gfx->setCursor(265, 15); 
            gfx->printf("%d%%", signalStrength);
            lastSignalStrength = signalStrength;
        }
    }

    // ==================================================================
    // PROSES RENDER ANGKA GLOW STANDARD FONT
    // ==================================================================
    if (speedValue != lastSpeedValue) {
        canvas->fillScreen(BLACK);
        
        for (int r = 85; r > 0; r -= 4) {
            uint16_t glowColor = BLACK;
            if (r > 65)       glowColor = 0x0004;
            else if (r > 45)  glowColor = 0x000A;
            else if (r > 25)  glowColor = 0x0012;
            else              glowColor = 0x011A;
            canvas->fillCircle(70, 30, r, glowColor);
        }

        char speedText[4];
        sprintf(speedText, "%03d", speedValue);

        canvas->setTextSize(6);
        canvas->setTextColor(NEON_GLOW);
        canvas->setCursor(16, 6);  canvas->print(speedText); 
        canvas->setCursor(20, 6);  canvas->print(speedText); 
        canvas->setCursor(16, 10); canvas->print(speedText); 
        canvas->setCursor(20, 10); canvas->print(speedText); 
        canvas->setCursor(18, 6);  canvas->print(speedText); 
        canvas->setCursor(18, 10); canvas->print(speedText); 
        canvas->setCursor(16, 8);  canvas->print(speedText); 
        canvas->setCursor(20, 8);  canvas->print(speedText); 
        
        canvas->setTextColor(WHITE);
        canvas->setCursor(18, 8); 
        canvas->print(speedText);

        gfx->draw16bitRGBBitmap(90, 45, canvas->getFramebuffer(), CANVAS_W, CANVAS_H);
        lastSpeedValue = speedValue;
    }

    // Neon Bar Gradasi
    int barWidth = map(speedValue, 0, 120, 0, 120); 
    for (int i = 0; i < barWidth; i++) {
        uint16_t segmentColor = GREEN;
        if (i >= 50 && i < 90) segmentColor = YELLOW;
        else if (i >= 90)      segmentColor = RED;
        gfx->drawFastVLine(100 + i, 114, 4, segmentColor);
    }
    if (barWidth < 120) {
        gfx->fillRect(100 + barWidth, 114, 120 - barWidth, 4, GLOW_BLUE_3);
    }

    // Sein Kuning
    if (currentSteerState == 1 && blinkState) {
        gfx->fillTriangle(25, 75, 55, 55, 55, 95, YELLOW);
    }
    if (currentSteerState == 2 && blinkState) {
        gfx->fillTriangle(295, 75, 265, 55, 265, 95, YELLOW);
    }

    delay(5); 
}
