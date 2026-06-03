#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include "gothic_font.h" // Memanggil font gothic kustom Anda

// ======================================================
// WAVESHARE ESP32-C6 1.47"
// PREMIUM GOTHIC STYLE DASHBOARD (TRANSPARENT SPRITE)
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
#define GREEN          0x07E0 
#define YELLOW         0xFFE0
#define GLOW_BLUE_3    0x0012 

Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, GFX_NOT_DEFINED);
Arduino_GFX *gfx = new Arduino_ST7789(bus, TFT_RST, 1, true, 172, 320, 34, 0, 34, 0);

// Canvas area tengah (Lebar disesuaikan agar 3 digit Gothic muat ideal)
#define CANVAS_W 140
#define CANVAS_H 60
Arduino_Canvas *canvas = new Arduino_Canvas(CANVAS_W, CANVAS_H, gfx);

int speedValue = 0; 
int lastSpeedValue = -1; 
int targetSpeed = 0;
bool blinkState = false;
unsigned long blinkTimer = 0;

// Fungsi menggambar background melingkar halus bawaan layar
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

    // KM/H TEXT STATIC
    gfx->setTextColor(WHITE); 
    gfx->setTextSize(2);
    gfx->setCursor(136, 125); 
    gfx->print("KM/H");

    pinMode(STEER_PIN, INPUT);
    pinMode(THROTTLE_PIN, INPUT);
}

void loop() {
    // Sinyal remot RC
    int steerPWM = pulseIn(STEER_PIN, HIGH, 20000);
    int throttlePWM = pulseIn(THROTTLE_PIN, HIGH, 20000);

    if (steerPWM == 0)     steerPWM = 1500;
    if (throttlePWM == 0)  throttlePWM = 1500; 

    // Konversi gas remot ke Kecepatan
    if (throttlePWM < 1490) {
        targetSpeed = map(throttlePWM, 1500, 1000, 0, 120);
        targetSpeed = constrain(targetSpeed, 0, 120);
    } else if (throttlePWM > 1510) {
        targetSpeed = map(throttlePWM, 1500, 2000, 0, 50);
        targetSpeed = constrain(targetSpeed, 0, 50);
    } else {
        targetSpeed = 0;
    }

    // Pergerakan spidometer dibuat mulus halus
    if (speedValue < targetSpeed) {
        speedValue += 4; 
        if (speedValue > targetSpeed) speedValue = targetSpeed;
    }
    if (speedValue > targetSpeed) {
        speedValue -= 4;
        if (speedValue < targetSpeed) speedValue = targetSpeed;
    }

    if (millis() - blinkTimer > 350) {
        blinkTimer = millis();
        blinkState = !blinkState;
    }

    gfx->fillRect(15, 50, 45, 55, BLACK);  
    gfx->fillRect(260, 50, 50, 55, BLACK); 

    // ==================================================================
    // LOGIKA MENEMPEL GAMBAR ANGKA GOTHIC DI ATAS PENDARYAN BIRU
    // ==================================================================
    if (speedValue != lastSpeedValue) {
        
        // 1. Bersihkan canvas internal
        canvas->fillScreen(BLACK);
        
        // 2. Gambar ulang pendaran biru melingkar persis di belakang angka
        for (int r = 85; r > 0; r -= 4) {
            uint16_t glowColor = BLACK;
            if (r > 65)       glowColor = 0x0004;
            else if (r > 45)  glowColor = 0x000A;
            else if (r > 25)  glowColor = 0x0012;
            else              glowColor = 0x011A;
            canvas->fillCircle(70, 30, r, glowColor);
        }

        // 3. Pecah angka menjadi Ratusan, Puluhan, Satuan
        int digit1 = speedValue / 100;          
        int digit2 = (speedValue / 10) % 10;    
        int digit3 = speedValue % 10;           

        // 4. Tempel potongan gambar angka berdampingan secara presisi
        // Koordinat X digeser seimbang (20, 55, 90) agar berada tepat di tengah lingkaran
        canvas->draw16bitRGBBitmap(20, 3, (uint16_t*)pgm_read_ptr(&gothicNumbers[digit1]), IMG_W, IMG_H);
        canvas->draw16bitRGBBitmap(55, 3, (uint16_t*)pgm_read_ptr(&gothicNumbers[digit2]), IMG_W, IMG_H);
        canvas->draw16bitRGBBitmap(90, 3, (uint16_t*)pgm_read_ptr(&gothicNumbers[digit3]), IMG_W, IMG_H);

        // 5. Dorong canvas memori utuh ke layar LCD (X: 90, Y: 45)
        gfx->draw16bitRGBBitmap(90, 45, canvas->getFramebuffer(), CANVAS_W, CANVAS_H);

        lastSpeedValue = speedValue;
    }

    // Neon bar melengkung bagian bawah
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

    // Lampu sein aktif
    if (steerPWM < 1400 && blinkState) gfx->fillTriangle(25, 75, 55, 55, 55, 95, YELLOW);
    if (steerPWM > 1600 && blinkState) gfx->fillTriangle(295, 75, 265, 55, 265, 95, YELLOW);

    delay(5);
}
