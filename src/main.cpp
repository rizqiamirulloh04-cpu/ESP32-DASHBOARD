#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// ======================================================
// WAVESHARE ESP32-C6 1.47"
// PERFECT CENTERED CANVAS SPEEDOMETER (NO TRUNCATION)
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

// GRADIENT BLUE BACKGROUND
#define GLOW_BLUE_1    0x0004 
#define GLOW_BLUE_2    0x000A 
#define GLOW_BLUE_3    0x0012 
#define GLOW_BLUE_4    0x011A 

Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, GFX_NOT_DEFINED);
Arduino_GFX *gfx = new Arduino_ST7789(bus, TFT_RST, 1, true, 172, 320, 34, 0, 34, 0);

// UKURAN KANVAS DIPERBARUI (Lebih lebar agar 3 digit angka muat sempurna)
#define CANVAS_W 160
#define CANVAS_H 64
Arduino_Canvas *canvas = new Arduino_Canvas(CANVAS_W, CANVAS_H, gfx);

int speedValue = 0; 
int lastSpeedValue = -1; 
int targetSpeed = 0;
bool blinkState = false;
unsigned long blinkTimer = 0;

// FUNGSI UTAMA: Menggambar Lingkaran Gradasi Latar Belakang Layar
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

    // Alokasikan memori untuk kanvas angka
    canvas->begin();

    drawStaticUI();

    pinMode(STEER_PIN, INPUT);
    pinMode(THROTTLE_PIN, INPUT);
}

void loop() {
    // Membaca PWM dari remot dengan pembatasan waktu agar tidak lag
    int steerPWM = pulseIn(STEER_PIN, HIGH, 20000);
    int throttlePWM = pulseIn(THROTTLE_PIN, HIGH, 20000);

    if (steerPWM == 0)     steerPWM = 1500;
    if (throttlePWM == 0)  throttlePWM = 1500; 

    // FILTER DAN AKURASI GAS (MAJU / MUNDUR)
    if (throttlePWM < 1490) {
        targetSpeed = map(throttlePWM, 1500, 1000, 0, 120);
        targetSpeed = constrain(targetSpeed, 0, 120);
    } else if (throttlePWM > 1510) {
        targetSpeed = map(throttlePWM, 1500, 2000, 0, 50);
        targetSpeed = constrain(targetSpeed, 0, 50);
    } else {
        targetSpeed = 0;
    }

    // SPEED SMOOTHING (Pergerakan angka naik turun dibuat mulus)
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

    // REFRESH AREA SEIN (Kanan & Kiri ujung layar)
    gfx->fillRect(15, 50, 45, 55, BLACK);  
    gfx->fillRect(260, 50, 50, 55, BLACK); 

    // ==================================================
    // PROSES DETAILED KANVAS ANGKA (CENTERED & FULL DIGIT)
    // ==================================================
    if (speedValue != lastSpeedValue) {
        
        // 1. Reset isi kanvas memori
        canvas->fillScreen(BLACK);
        
        // 2. Gambar ulang potongan pusat lingkaran gradasi biru di dalam kanvas
        // Koordinat disesuaikan dengan posisi tengah baru (Pusat Layar 160 -> Pusat Kanvas 80)
        for (int r = 85; r > 0; r -= 4) {
            uint16_t glowColor = BLACK;
            if (r > 65)       glowColor = GLOW_BLUE_1;
            else if (r > 45)  glowColor = GLOW_BLUE_2;
            else if (r > 25)  glowColor = GLOW_BLUE_3;
            else              glowColor = GLOW_BLUE_4;
            
            canvas->fillCircle(80, 32, r, glowColor);
        }

        // 3. Cetak angka dengan posisi X yang sudah digeser ke tengah kanvas (X: 18)
        canvas->setTextColor(GREEN); 
        canvas->setTextSize(7); 
        canvas->setCursor(18, 5); 

        // Cetak format 3-digit agar presisi
        if (speedValue < 10) {
            canvas->print("00");
        } else if (speedValue < 100) {
            canvas->print("0");
        }
        canvas->print(speedValue);

        // 4. Kirim bitmap kanvas ke layar utama secara pas (X posisi digeser ke 80 agar simetris)
        gfx->draw16bitRGBBitmap(80, 42, canvas->getFramebuffer(), CANVAS_W, CANVAS_H);

        lastSpeedValue = speedValue;
    }

    // DYNAMIC NEON BAR (Garis parameter warna di bawah angka)
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

    // LOGIKA LAMPU SEIN KANAN / KIRI
    if (steerPWM < 1400 && blinkState) {
        gfx->fillTriangle(25, 75, 55, 55, 55, 95, YELLOW);
    }
    if (steerPWM > 1600 && blinkState) {
        gfx->fillTriangle(295, 75, 265, 55, 265, 95, YELLOW);
    }

    delay(5);
}
