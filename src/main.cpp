#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// ======================================================
// WAVESHARE ESP32-C6 1.47"
// CYBERPUNK RACING DASHBOARD (PREMIUM GRADIENT)
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

// ================= CYBERPUNK COLOR PALETTE =================
#define BLACK          0x0000
#define WHITE          0xFFFF
#define RED            0xF800
#define YELLOW         0xFFE0

// Warna Utama Angka & Bar (Oranye Racing Neon)
#define NEON_ORANGE    0xFC00 

// Gradasi Biru Tua yang Melebur Halus ke Hitam (Kunci efek tidak ngeblok)
#define GLOW_BLUE_1    0x0004 // Sangat gelap (luar) - hampir menyatu dengan hitam
#define GLOW_BLUE_2    0x000A // Biru malam redup
#define GLOW_BLUE_3    0x0012 // Biru malam sedang
#define GLOW_BLUE_4    0x011A // Pusat pendaran (agak terang tapi tidak solid)

// ======================================================
// DISPLAY CONFIGURATION
// ======================================================
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
    1,     // Landscape mode (320x172)
    true,  // IPS = true
    172,   
    320,   
    34,    
    0,     
    34,    
    0      
);

// ======================================================
// VARIABLES
// ======================================================
int speedValue = 0; 
int targetSpeed = 0;

bool blinkState = false;
unsigned long blinkTimer = 0;

// ======================================================
// SMOOTH RADIAL GLOW (Membuat lingkaran gradasi tipis-tipis)
// ======================================================
void drawCyberpunkGlow()
{
    // Kita buat jangkauan lingkaran lebih luas (sampai radius 90) 
    // tapi dengan lompatan tipis per 2 piksel agar transisinya halus (anti-kotak)
    for (int r = 90; r > 0; r -= 2) 
    {
        uint16_t glowColor = BLACK;
        
        if (r > 70)       glowColor = GLOW_BLUE_1; // Luar sekali (samar-samar)
        else if (r > 45)  glowColor = GLOW_BLUE_2; // Mulai membiru
        else if (r > 25)  glowColor = GLOW_BLUE_3; // Biru penengah
        else              glowColor = GLOW_BLUE_4; // Pusat di belakang angka
        
        gfx->fillCircle(160, 80, r, glowColor);
    }
}

// ======================================================
// STATIC UI
// ======================================================
void drawStaticUI()
{
    gfx->fillScreen(BLACK);

    // 1. Gambar pendaran gradasi biru tua cyberpunk
    drawCyberpunkGlow();

    // 2. KM/H TEXT
    gfx->setTextColor(WHITE); // Putih bersih agar kontras di atas biru
    gfx->setTextSize(2);
    gfx->setCursor(136, 125); 
    gfx->print("KM/H");
}

// ======================================================
// SETUP
// ======================================================
void setup()
{
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

// ======================================================
// LOOP
// ======================================================
void loop()
{
    int steerPWM = pulseIn(STEER_PIN, HIGH, 25000);
    int throttlePWM = pulseIn(THROTTLE_PIN, HIGH, 25000);

    if (steerPWM == 0)     steerPWM = 1500;
    if (throttlePWM == 0)  throttlePWM = 1500; 

    // LOGIKA FILTER GAS
    if (throttlePWM < 1490) {
        targetSpeed = map(throttlePWM, 1500, 1000, 0, 120);
        targetSpeed = constrain(targetSpeed, 0, 120);
    } else if (throttlePWM > 1510) {
        targetSpeed = map(throttlePWM, 1500, 2000, 0, 50);
        targetSpeed = constrain(targetSpeed, 0, 50);
    } else {
        targetSpeed = 0;
    }

    // SMOOTHING
    if (speedValue < targetSpeed) {
        speedValue += 4; 
        if (speedValue > targetSpeed) speedValue = targetSpeed;
    }
    if (speedValue > targetSpeed) {
        speedValue -= 4;
        if (speedValue < targetSpeed) speedValue = targetSpeed;
    }

    // TIMER KEDIP SEIN
    if (millis() - blinkTimer > 350) {
        blinkTimer = millis();
        blinkState = !blinkState;
    }

    // REFRESH AREA SEIN (Di area hitam samping)
    gfx->fillRect(15, 50, 45, 55, BLACK);  
    gfx->fillRect(260, 50, 50, 55, BLACK); 

    // ==================================================
    // SPEED NUMBER (ORANGE ON DEEP BLUE)
    // ==================================================
    // Latar belakang teks dikunci ke GLOW_BLUE_4 (warna pusat pendaran)
    gfx->setTextColor(NEON_ORANGE, GLOW_BLUE_4);
    gfx->setTextSize(7); 
    gfx->setCursor(105, 50); 

    if (speedValue < 10) {
        gfx->print("00");
    } else if (speedValue < 100) {
        gfx->print("0");
    }
    gfx->print(speedValue);

    // ==================================================
    // DYNAMIC NEON SPEED BAR (Neon Berjalan)
    // ==================================================
    int barWidth = map(speedValue, 0, 120, 0, 120); 

    for (int i = 0; i < barWidth; i++) 
    {
        uint16_t segmentColor;
        if (i < 50)       segmentColor = NEON_ORANGE; // Dominan oranye keren
        else if (i < 90)  segmentColor = YELLOW;      // Transisi kuning
        else              segmentColor = RED;         // Ujungnya merah saat top speed
        
        gfx->drawFastVLine(100 + i, 114, 4, segmentColor);
    }

    // Sisa bar dihapus dengan warna GLOW_BLUE_3 (warna area bawah lingkaran)
    if (barWidth < 120) {
        gfx->fillRect(100 + barWidth, 114, 120 - barWidth, 4, GLOW_BLUE_3);
    }

    // ==================================================
    // SEIN (Tetap Kuning Neon)
    // ==================================================
    if (steerPWM < 1400 && blinkState) {
        gfx->fillTriangle(25, 75, 55, 55, 55, 95, YELLOW);
    }
    if (steerPWM > 1600 && blinkState) {
        gfx->fillTriangle(295, 75, 265, 55, 265, 95, YELLOW);
    }

    delay(15);
}
