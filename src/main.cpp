#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// ======================================================
// WAVESHARE ESP32-C6 1.47"
// PERFECT TRANSPARENT GREEN SPEEDOMETER (NO MORE BOX)
// ======================================================

#define TFT_BL 22
#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS   14
#define TFT_DC   15
#define TFT_RST  21

#define STEER_PIN    1
#define THROTTLE_PIN 2

// COLORS
#define BLACK          0x0000
#define WHITE          0xFFFF
#define RED            0xF800
#define GREEN          0x07E0 // Hijau Terang Utama
#define YELLOW         0xFFE0

// GRADIENT BLUE BACKGROUND
#define GLOW_BLUE_1    0x0004 
#define GLOW_BLUE_2    0x000A 
#define GLOW_BLUE_3    0x0012 
#define GLOW_BLUE_4    0x011A 

Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, GFX_NOT_DEFINED);
Arduino_GFX *gfx = new Arduino_ST7789(bus, TFT_RST, 1, true, 172, 320, 34, 0, 34, 0);

int speedValue = 0; 
int lastSpeedValue = -1; // Menyimpan data angka sebelumnya untuk deteksi perubahan
int targetSpeed = 0;
bool blinkState = false;
unsigned long blinkTimer = 0;

// FUNGSI GAMBAR PENDARAN UTAMA
void drawCyberpunkGlow() {
    for (int r = 85; r > 0; r -= 3) {
        uint16_t glowColor = BLACK;
        if (r > 65)       glowColor = GLOW_BLUE_1;
        else if (r > 45)  glowColor = GLOW_BLUE_2;
        else if (r > 25)  glowColor = GLOW_BLUE_3;
        else              glowColor = GLOW_BLUE_4;
        
        gfx->fillCircle(160, 80, r, glowColor);
    }
}

// FUNGSI KHUSUS: Bersihkan sisa angka dengan menimpa pendaran di pusat saja
void refreshCenterGlow() {
    for (int r = 45; r > 0; r -= 3) {
        uint16_t glowColor = GLOW_BLUE_3;
        if (r <= 25) glowColor = GLOW_BLUE_4;
        gfx->fillCircle(160, 80, r, glowColor);
    }
}

// STATIC UI
void drawStaticUI() {
    gfx->fillScreen(BLACK);
    drawCyberpunkGlow();

    // KM/H TEXT
    gfx->setTextColor(WHITE); 
    gfx->setTextSize(2);
    gfx->setCursor(136, 125); 
    gfx->print("KM/H");
}

void setup() {
    Serial.begin(115200);
    ledcAttach(TFT_BL, 5000, 8);
    ledcWrite(TFT_BL, 128); 

    gfx->begin();
    gfx->invertDisplay(false);
    gfx->setRotation(1); 

    drawStaticUI();

    pinMode(STEER_PIN, INPUT);
    pinMode(THROTTLE_PIN, INPUT);
}

void loop() {
    int steerPWM = pulseIn(STEER_PIN, HIGH, 25000);
    int throttlePWM = pulseIn(THROTTLE_PIN, HIGH, 25000);

    if (steerPWM == 0)     steerPWM = 1500;
    if (throttlePWM == 0)  throttlePWM = 1500; 

    // FILTER GAS
    if (throttlePWM < 1490) {
        targetSpeed = map(throttlePWM, 1500, 1000, 0, 120);
        targetSpeed = constrain(targetSpeed, 0, 120);
    } else if (throttlePWM > 1510) {
        targetSpeed = map(throttlePWM, 1500, 2000, 0, 50);
        targetSpeed = constrain(targetSpeed, 0, 50);
    } else {
        targetSpeed = 0;
    }

    // SPEED SMOOTHING
    if (speedValue < targetSpeed) {
        speedValue += 4; 
        if (speedValue > targetSpeed) speedValue = targetSpeed;
    }
    if (speedValue > targetSpeed) {
        speedValue -= 4;
        if (speedValue < targetSpeed) speedValue = targetSpeed;
    }

    // TIMER KEDIP
    if (millis() - blinkTimer > 350) {
        blinkTimer = millis();
        blinkState = !blinkState;
    }

    // REFRESH AREA SEIN
    gfx->fillRect(15, 50, 45, 55, BLACK);  
    gfx->fillRect(260, 50, 50, 55, BLACK); 

    // ==================================================
    // LOGIKA ANTI-KOTAK (TRANSPARENT RENDERING)
    // ==================================================
    if (speedValue != lastSpeedValue) {
        // 1. Bersihkan sisa angka lama menggunakan pendaran biru melingkar asli
        refreshCenterGlow();

        // 2. Set warna teks ke HIJAU TERANG tanpa warna latar belakang (Transparan)
        gfx->setTextColor(GREEN); 
        gfx->setTextSize(7); 
        gfx->setCursor(105, 50); 

        if (speedValue < 10) {
            gfx->print("00");
        } else if (speedValue < 100) {
            gfx->print("0");
        }
        gfx->print(speedValue);

        // Simpan status angka terakhir
        lastSpeedValue = speedValue;
    }

    // DYNAMIC NEON BAR
    int barWidth = map(speedValue, 0, 120, 0, 120); 
    for (int i = 0; i < barWidth; i++) {
        uint16_t segmentColor;
        if (i < 50)       segmentColor = GREEN;  
        else if (i < 90)  segmentColor = YELLOW; 
        else              segmentColor = RED;    
        
        gfx->drawFastVLine(100 + i, 114, 4, segmentColor);
    }

    if (barWidth < 120) {
        gfx->fillRect(100 + barWidth, 114, 120 - barWidth, 4, GLOW_BLUE_3);
    }

    // SEIN
    if (steerPWM < 1400 && blinkState) {
        gfx->fillTriangle(25, 75, 55, 55, 55, 95, YELLOW);
    }
    if (steerPWM > 1600 && blinkState) {
        gfx->fillTriangle(295, 75, 265, 55, 265, 95, YELLOW);
    }

    delay(15);
}
