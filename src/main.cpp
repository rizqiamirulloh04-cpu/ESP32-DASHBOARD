#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// ======================================================================
// WAVESHARE ESP32-C6 1.47" - STANDARD TEXT DASHBOARD (NO BITMAP)
// ======================================================================

#define TFT_BL 22
#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS   14
#define TFT_DC   15
#define TFT_RST  21

#define STEER_PIN    1
#define THROTTLE_PIN 2

// COLORS (Format RGB565 16-Bit)
#define BLACK          0x0000
#define WHITE          0xFFFF
#define RED            0xF800
#define GREEN          0x07E0 
#define YELLOW         0xFFE0
#define GLOW_BLUE_3    0x0012 

// INITIALISASI HARDWARE DISPLAY & KANVAS
Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, GFX_NOT_DEFINED);
Arduino_GFX *gfx = new Arduino_ST7789(bus, TFT_RST, 1, true, 172, 320, 34, 0, 34, 0);

// Canvas internal anti-flicker (Untuk area angka agar transisinya mulus)
#define CANVAS_W 140
#define CANVAS_H 60
Arduino_Canvas *canvas = new Arduino_Canvas(CANVAS_W, CANVAS_H, gfx);

int speedValue = 0; 
int lastSpeedValue = -1; 
int targetSpeed = 0;
bool blinkState = false;
unsigned long blinkTimer = 0;
int lastSteerState = 0; 

// Fungsi menggambar pendaran lingkaran biru khas dasbor cyberpunk
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
    
    // Mengatur backlight layar ST7789 menggunakan fitur LEDC ESP32-C6
    ledcAttach(TFT_BL, 5000, 8);
    ledcWrite(TFT_BL, 128); 

    gfx->begin();
    gfx->invertDisplay(false);
    gfx->setRotation(1); // Set ke mode Landscape

    canvas->begin();
    gfx->fillScreen(BLACK);
    drawCyberpunkGlow();

    // Teks KM/H Statis di bawah lingkaran angka
    gfx->setTextColor(WHITE); 
    gfx->setTextSize(2);
    gfx->setCursor(136, 125); 
    gfx->print("KM/H");

    pinMode(STEER_PIN, INPUT);
    pinMode(THROTTLE_PIN, INPUT);
}

void loop() {
    // Membaca sinyal PWM dari Receiver remot RC
    int steerPWM = pulseIn(STEER_PIN, HIGH, 20000);
    int throttlePWM = pulseIn(THROTTLE_PIN, HIGH, 20000);

    // Proteksi failsafe: Jika tidak ada sinyal, asumsikan posisi netral (1500us)
    if (steerPWM == 0)     steerPWM = 1500;
    if (throttlePWM == 0)  throttlePWM = 1500; 

    // Filter logika konversi gas remot ke Kecepatan Target
    if (throttlePWM < 1490) { // Maju
        targetSpeed = map(throttlePWM, 1500, 1000, 0, 120);
        targetSpeed = constrain(targetSpeed, 0, 120);
    } else if (throttlePWM > 1510) { // Mundur
        targetSpeed = map(throttlePWM, 1500, 2000, 0, 50);
        targetSpeed = constrain(targetSpeed, 0, 50);
    } else {
        targetSpeed = 0; // Netral
    }

    // Efek pergerakan transisi angka dibuat berangsur/mulus (Smoothing effect)
    if (speedValue < targetSpeed) {
        speedValue += 4; 
        if (speedValue > targetSpeed) speedValue = targetSpeed;
    }
    if (speedValue > targetSpeed) {
        speedValue -= 4;
        if (speedValue < targetSpeed) speedValue = targetSpeed;
    }

    // Mengatur interval waktu kedipan sein (350 milidetik)
    bool toggleBlink = false;
    if (millis() - blinkTimer > 350) {
        blinkTimer = millis();
        blinkState = !blinkState;
        toggleBlink = true; 
    }

    // Deteksi arah kemudi: 0 = Lurus, 1 = Belok Kiri, 2 = Belok Kanan
    int currentSteerState = 0; 
    if (steerPWM < 1400)      currentSteerState = 1;
    else if (steerPWM > 1600) currentSteerState = 2;

    // Manajemen penghapusan area sein agar tidak berkedip redup akibat loop konstan
    if (currentSteerState != lastSteerState || toggleBlink) {
        if (!blinkState || currentSteerState == 0) {
            gfx->fillRect(15, 50, 45, 55, BLACK);  
            gfx->fillRect(260, 50, 50, 55, BLACK); 
        }
        lastSteerState = currentSteerState;
    }

    // ==================================================================
    // PROSES RENDAER ANGKA MENGGUNAKAN TEKS STANDAR (FONT BAWAAN)
    // ==================================================================
    if (speedValue != lastSpeedValue) {
        
        // 1. Bersihkan area Kanvas internal
        canvas->fillScreen(BLACK);
        
        // 2. Gambar ulang efek pendaran lingkaran di Kanvas agar menyatu alami dengan background
        for (int r = 85; r > 0; r -= 4) {
            uint16_t glowColor = BLACK;
            if (r > 65)       glowColor = 0x0004;
            else if (r > 45)  glowColor = 0x000A;
            else if (r > 25)  glowColor = 0x0012;
            else              glowColor = 0x011A;
            canvas->fillCircle(70, 30, r, glowColor);
        }

        // 3. Format angka bulat menjadi format 3 digit (misal: 0 -> "000", 5 -> "005")
        char speedText[4];
        sprintf(speedText, "%03d", speedValue);

        // 4. Gambar teks angka standar ke kanvas (Ukuran 6 agar besar dan terbaca jelas)
        canvas->setTextColor(WHITE);
        canvas->setTextSize(6);
        canvas->setCursor(18, 8); 
        canvas->print(speedText);

        // 5. Kirim gambar hasil gabungan kanvas utuh ke tengah layar LCD utama
        gfx->draw16bitRGBBitmap(90, 45, canvas->getFramebuffer(), CANVAS_W, CANVAS_H);

        lastSpeedValue = speedValue;
    }

    // Indikator Neon Bar Gradasi Warna (Hijau -> Kuning -> Merah)
    int barWidth = map(speedValue, 0, 120, 0, 120); 
    for (int i = 0; i < barWidth; i++) {
        uint16_t segmentColor = GREEN;
        if (i >= 50 && i < 90) segmentColor = YELLOW;
        else if (i >= 90)      segmentColor = RED;
        gfx->drawFastVLine(100 + i, 114, 4, segmentColor);
    }
    // Sisa bar yang tidak aktif diberi warna redup (pendaran dasar)
    if (barWidth < 120) {
        gfx->fillRect(100 + barWidth, 114, 120 - barWidth, 4, GLOW_BLUE_3);
    }

    // Gambar Segitiga Lampu Sein Kuning (Menyala stabil saat siklus blinkState aktif)
    if (currentSteerState == 1 && blinkState) {
        gfx->fillTriangle(25, 75, 55, 55, 55, 95, YELLOW);
    }
    if (currentSteerState == 2 && blinkState) {
        gfx->fillTriangle(295, 75, 265, 55, 265, 95, YELLOW);
    }

    delay(5); 
}
