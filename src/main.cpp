#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// Mengimport Font Kustom Bergaya Miring & Tebal dari Library GFX
#include <Fonts/FreeSansBoldItalic18pt7b.h>

// ======================================================================
// WAVESHARE ESP32-C6 1.47" - ITALIC SPORTY PREMIUM DASHBOARD
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

// COLOR PALETTE (RGB565 16-Bit)
#define BLACK          0x0000
#define WHITE          0xFFFF
#define RED            0xF800
#define GREEN          0x07E0 
#define YELLOW         0xFFE0
#define CYAN           0x07FF
#define DARK_BLUE      0x0010
#define GRAY           0x4208

// INITIALISASI HARDWARE DISPLAY
Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, GFX_NOT_DEFINED);
Arduino_GFX *gfx = new Arduino_ST7789(bus, TFT_RST, 1, true, 172, 320, 34, 0, 34, 0);

// Canvas internal untuk area tengah (Speedometer Busur + Angka Miring)
#define CANVAS_W 160
#define CANVAS_H 100
Arduino_Canvas *canvas = new Arduino_Canvas(CANVAS_W, CANVAS_H, gfx);

int speedValue = 0; 
int lastSpeedValue = -1; 
int targetSpeed = 0;
int topSpeed = 0;
bool blinkState = false;
unsigned long blinkTimer = 0;
int lastSteerState = 0; 

// Variabel Sensor
int batteryPercent = 89;
int signalDbm = -67;
int temperature = 38;
unsigned long sensorTimer = 0;

// Fungsi menggambar elemen dekorasi statis saat pertama kali menyala
void drawStaticDashboard() {
    gfx->fillScreen(BLACK);

    // 1. Header Atas
    gfx->setTextColor(CYAN);
    gfx->setTextSize(1);
    gfx->setCursor(125, 8);
    gfx->print("SPORT MODE");

    // Garis dekorasi atas mirip gambar asli
    gfx->drawLine(10, 20, 110, 20, CYAN);
    gfx->drawLine(110, 20, 120, 10, CYAN);
    gfx->drawLine(120, 10, 200, 10, CYAN);
    gfx->drawLine(200, 10, 210, 20, CYAN);
    gfx->drawLine(210, 20, 310, 20, CYAN);

    // 2. Info Sinyal & Baterai Atas
    gfx->setTextColor(WHITE);
    gfx->setCursor(15, 8);
    gfx->printf("%d dBm", signalDbm);
    
    gfx->setCursor(275, 8);
    gfx->printf("%d%%", batteryPercent);

    // 3. Label Lampu Sein
    gfx->setTextSize(1);
    gfx->setTextColor(GRAY);
    gfx->setCursor(20, 75);  gfx->print("LEFT");
    gfx->setCursor(275, 75); gfx->print("RIGHT");

    // 4. Footer Bawah (Top Speed & Temperature)
    gfx->drawRect(115, 133, 90, 24, GRAY);
    gfx->setTextColor(CYAN);
    gfx->setCursor(130, 137); gfx->print("TOP SPEED");

    gfx->setTextColor(WHITE);
    gfx->setCursor(265, 140); gfx->printf("%d 'C", temperature);
}

void setup() {
    Serial.begin(115200);
    
    ledcAttach(TFT_BL, 5000, 8);
    ledcWrite(TFT_BL, 140); 

    gfx->begin();
    gfx->invertDisplay(false);
    gfx->setRotation(1); 

    canvas->begin();
    
    // Set Font kustom untuk kanvas internal sejak awal
    canvas->setFont(&FreeSansBoldItalic18pt7b);

    drawStaticDashboard();

    pinMode(STEER_PIN, INPUT);
    pinMode(THROTTLE_PIN, INPUT);
    pinMode(BATTERY_PIN, INPUT);
    pinMode(SIGNAL_PIN, INPUT);
}

void loop() {
    // Sinyal PWM dari Remote RC
    int steerPWM = pulseIn(STEER_PIN, HIGH, 20000);
    int throttlePWM = pulseIn(THROTTLE_PIN, HIGH, 20000);

    if (steerPWM == 0)     steerPWM = 1500;
    if (throttlePWM == 0)  throttlePWM = 1500; 

    // Logika gas
    if (throttlePWM < 1490) { 
        targetSpeed = map(throttlePWM, 1500, 1000, 0, 120);
        targetSpeed = constrain(targetSpeed, 0, 120);
    } else if (throttlePWM > 1510) { 
        targetSpeed = map(throttlePWM, 1500, 2000, 0, 50);
        targetSpeed = constrain(targetSpeed, 0, 50);
    } else {
        targetSpeed = 0; 
    }

    // Pergerakan angka smooth
    if (speedValue < targetSpeed) speedValue += 4;
    if (speedValue > targetSpeed) speedValue -= 4;
    if (abs(speedValue - targetSpeed) < 4) speedValue = targetSpeed;

    // Catat Top Speed tertinggi selama dimainkan
    if (speedValue > topSpeed) {
        topSpeed = speedValue;
        gfx->fillRect(120, 146, 80, 10, BLACK);
        gfx->setTextColor(WHITE);
        gfx->setCursor(138, 146);
        gfx->printf("%d KM/H", topSpeed);
    }

    // Timer Kedipan Lampu Sein
    bool toggleBlink = false;
    if (millis() - blinkTimer > 350) {
        blinkTimer = millis();
        blinkState = !blinkState;
        toggleBlink = true; 
    }

    // Arah kemudi
    int currentSteerState = 0; 
    if (steerPWM < 1400)      currentSteerState = 1; // Kiri
    else if (steerPWM > 1600) currentSteerState = 2; // Kanan

    // Logika hapus/gambar ulang lampu sein
    if (currentSteerState != lastSteerState || toggleBlink) {
        if (!blinkState || currentSteerState == 0) {
            gfx->fillRect(15, 45, 35, 25, BLACK);  
            gfx->fillRect(270, 45, 35, 25, BLACK); 
            
            gfx->fillTriangle(35, 57, 45, 45, 45, 69, DARK_BLUE);
            gfx->fillRect(15, 52, 20, 10, DARK_BLUE);
            gfx->fillTriangle(285, 57, 275, 45, 275, 69, DARK_BLUE);
            gfx->fillRect(285, 52, 20, 10, DARK_BLUE);
        }
        lastSteerState = currentSteerState;
    }

    if (currentSteerState == 1 && blinkState) {
        gfx->fillTriangle(35, 57, 45, 45, 45, 69, GREEN);
        gfx->fillRect(15, 52, 20, 10, GREEN);
    }
    if (currentSteerState == 2 && blinkState) {
        gfx->fillTriangle(285, 57, 275, 45, 275, 69, GREEN);
        gfx->fillRect(285, 52, 20, 10, GREEN);
    }

    // Update data sensor bat & sinyal
    if (millis() - sensorTimer > 500) {
        sensorTimer = millis();
        int rawBat = analogRead(BATTERY_PIN);
        batteryPercent = map(rawBat, 0, 4095, 0, 100);
        batteryPercent = constrain(batteryPercent, 0, 100);
        
        gfx->fillRect(275, 8, 35, 10, BLACK);
        gfx->setTextColor(WHITE);
        gfx->setTextSize(1);
        gfx->setCursor(275, 8);
        gfx->printf("%d%%", batteryPercent);
    }

    // ==================================================================
    // RENDER AREA TENGAH (SPEEDOMETER BUSUR + ANGKA TEKS MIRING)
    // ==================================================================
    if (speedValue != lastSpeedValue) {
        canvas->fillScreen(BLACK);

        // 1. Menggambar Busur Mengikuti Nilai Kecepatan (Speed Arc)
        int arcBlankDegree = 120; 
        int speedArcValue = map(speedValue, 0, 120, 0, 360 - arcBlankDegree);

        for (int r = 48; r > 44; r--) {
            canvas->drawArc(80, 50, r, r - 1, 150, 390, GRAY); // Latar dasar
            if (speedArcValue > 0) {
                canvas->drawArc(80, 50, r, r - 1, 150, 150 + speedArcValue, CYAN); // Jalur aktif
            }
        }

        // 2. Format Teks 3 Digit
        char speedText[4];
        sprintf(speedText, "%03d", speedValue);

        // 3. Render Angka Utama Menggunakan Font Miring Kustom
        // Menggunakan ukuran/skala teks 2 agar tebal dan pas di tengah lingkaran busur
        canvas->setTextSize(2); 

        // Efek bayangan pendaran luar (Glow effect sekeliling angka)
        canvas->setTextColor(DARK_BLUE);
        canvas->setCursor(21, 59); canvas->print(speedText);
        canvas->setCursor(23, 59); canvas->print(speedText);
        canvas->setCursor(22, 58); canvas->print(speedText);
        canvas->setCursor(22, 60); canvas->print(speedText);

        // Angka Utama Putih Tajam dengan Font Miring Kustom
        canvas->setTextColor(WHITE);
        canvas->setCursor(22, 59); // Sumbu Y disesuaikan (59) karena baseline GFX font berbeda dengan font standar
        canvas->print(speedText);

        // 4. Mengembalikan Font ke Mode Standar Khusus untuk Teks KM/H kecil di bawahnya
        canvas->setFont(NULL); 
        canvas->setTextSize(1);
        canvas->setTextColor(CYAN);
        canvas->setCursor(67, 72);
        canvas->print("KM/H");

        // Kembalikan ke font kustom miring untuk siklus render loop berikutnya
        canvas->setFont(&FreeSansBoldItalic18pt7b);

        // 5. RPM Bar Kecil Horizontal di bawah tulisan KM/H
        int rpmWidth = map(speedValue, 0, 120, 0, 80);
        canvas->fillRect(40, 88, 80, 3, DARK_BLUE); 
        if (rpmWidth > 0) {
            uint16_t rpmColor = (speedValue > 90) ? RED : (speedValue > 50 ? YELLOW : GREEN);
            canvas->fillRect(40, 88, rpmWidth, 3, rpmColor); 
        }

        // Kirim hasil akhir gabungan kanvas utuh ke tengah layar utama
        gfx->draw16bitRGBBitmap(80, 25, canvas->getFramebuffer(), CANVAS_W, CANVAS_H);

        lastSpeedValue = speedValue;
    }

    delay(5); 
}
