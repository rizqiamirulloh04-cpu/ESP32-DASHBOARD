#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// ======================================================================
// WAVESHARE ESP32-C6 1.47" - FULL PREMIUM CYBERPUNK DASHBOARD (CALIBRATED)
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
#define DARK_BLUE      0x0012
#define GRAY           0x5AEB

// INITIALISASI HARDWARE DISPLAY
Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, GFX_NOT_DEFINED);
Arduino_GFX *gfx = new Arduino_ST7789(bus, TFT_RST, 1, true, 172, 320, 34, 0, 34, 0);

// Canvas Utama Ukuran Penuh (320x172) untuk Double Buffering (Anti-Kedip)
Arduino_Canvas *canvas = new Arduino_Canvas(320, 172, gfx);

int speedValue = 0; 
int targetSpeed = 0;
int topSpeed = 0;
bool blinkState = false;
unsigned long blinkTimer = 0;
int currentSteerState = 0;

// Variabel Sensor Analog
int batteryPercent = 89;
int signalDbm = -67;
int temperature = 38;
unsigned long sensorTimer = 0;

// ======================================================================
// FUNGSI UNTUK MENGGAMBAR ELEMEN GRAFIS CUSTOM (IKON & CIRI KHAS)
// ======================================================================

// Menggambar ikon Sinyal Wifi/Batang di kiri atas
void drawSignalIcon(int x, int y) {
    canvas->fillCircle(x + 10, y + 12, 2, CYAN);
    canvas->drawArc(x + 10, y + 12, 5, 4, 220, 320, CYAN);
    canvas->drawArc(x + 10, y + 12, 9, 8, 220, 320, CYAN);
}

// Menggambar ikon Baterai Kotak Hijau di kanan atas
void drawBatteryIcon(int x, int y) {
    canvas->drawRect(x, y + 3, 18, 10, GRAY);
    canvas->fillRect(x + 18, y + 6, 2, 4, GRAY);
    canvas->fillRect(x + 2, y + 5, 14, 6, GREEN);
}

// Menggambar bentuk ikon Setir Mobil kecil (di bawah lampu sein)
void drawSteeringIcon(int x, int y) {
    canvas->drawCircle(x, y, 12, GRAY);
    canvas->drawCircle(x, y, 2, GRAY);
    canvas->drawLine(x - 11, y, x + 11, y, GRAY);
    canvas->drawLine(x, y + 2, x, y + 11, GRAY);
}

// Menggambar ikon Termometer Suhu di kanan bawah
void drawThermometerIcon(int x, int y) {
    canvas->drawCircle(x + 4, y + 12, 4, CYAN);
    canvas->fillRect(x + 3, y, 3, 10, CYAN);
    canvas->fillRect(x + 4, y + 3, 1, 10, RED);
}

// ======================================================================
// MAIN SETUP
// ======================================================================
void setup() {
    Serial.begin(115200);
    
    ledcAttach(TFT_BL, 5000, 8);
    ledcWrite(TFT_BL, 150); // Set kecerahan layar tinggi

    gfx->begin();
    gfx->invertDisplay(false);
    gfx->setRotation(1); // Mode Landscape (Tidur)

    canvas->begin();
    canvas->fillScreen(BLACK);

    pinMode(STEER_PIN, INPUT);
    pinMode(THROTTLE_PIN, INPUT);
    pinMode(BATTERY_PIN, INPUT);
    pinMode(SIGNAL_PIN, INPUT);
}

// ======================================================================
// MAIN LOOP
// ======================================================================
void loop() {
    // 1. Membaca Sinyal PWM dari Remote RC
    int steerPWM = pulseIn(STEER_PIN, HIGH, 20000);
    int throttlePWM = pulseIn(THROTTLE_PIN, HIGH, 20000);

    if (steerPWM == 0)     steerPWM = 1500;
    if (throttlePWM == 0)  throttlePWM = 1500; 

    // Logika pembacaan Gas
    if (throttlePWM < 1490) { 
        targetSpeed = map(throttlePWM, 1500, 1000, 0, 120);
        targetSpeed = constrain(targetSpeed, 0, 120);
    } else if (throttlePWM > 1510) { 
        targetSpeed = map(throttlePWM, 1500, 2000, 0, 50);
        targetSpeed = constrain(targetSpeed, 0, 50);
    } else {
        targetSpeed = 0; 
    }

    // Pergerakan akselerasi angka smooth
    if (speedValue < targetSpeed) speedValue += 4;
    if (speedValue > targetSpeed) speedValue -= 4;
    if (abs(speedValue - targetSpeed) < 4) speedValue = targetSpeed;

    if (speedValue > topSpeed) topSpeed = speedValue;

    // Timer Deteksi Lampu Sein (Kedipan)
    if (millis() - blinkTimer > 350) {
        blinkTimer = millis();
        blinkState = !blinkState;
    }

    // Kondisi kemudi (Lampu Sein)
    currentSteerState = 0; 
    if (steerPWM < 1400)      currentSteerState = 1; // Kiri
    else if (steerPWM > 1600) currentSteerState = 2; // Kanan

    // Update data sensor berkala dari Pin Analog ESP32-C6
    if (millis() - sensorTimer > 500) {
        sensorTimer = millis();
        int rawBat = analogRead(BATTERY_PIN);
        batteryPercent = map(rawBat, 0, 4095, 0, 100);
        batteryPercent = constrain(batteryPercent, 0, 100);
    }

    // ==================================================================
    // RENDER UTAMA KE INTERNAL CANVAS (PROSES DRAWING GRAPHICS)
    // ==================================================================
    canvas->fillScreen(BLACK); // Bersihkan memori kanvas sebelum digambar ulang

    // ---- A. GARIS DEKORASI HEADER ATAS ----
    canvas->setTextColor(CYAN);
    canvas->setTextSize(1);
    canvas->setCursor(130, 8);
    canvas->print("SPORT MODE");

    canvas->drawLine(10, 20, 115, 20, CYAN);
    canvas->drawLine(115, 20, 123, 11, CYAN);
    canvas->drawLine(123, 11, 197, 11, CYAN);
    canvas->drawLine(197, 11, 205, 20, CYAN);
    canvas->drawLine(205, 20, 310, 20, CYAN);

    // ---- B. INFO SINYAL & BATERAI ATAS ----
    drawSignalIcon(15, 5);
    canvas->setTextColor(WHITE);
    canvas->setCursor(40, 8);
    canvas->printf("%d dBm", signalDbm);
    
    drawBatteryIcon(250, 5);
    canvas->setCursor(275, 8);
    canvas->printf("%d%%", batteryPercent);

    // ---- C. INDIKATOR LAMPU SEIN (PANAH SOLID KANAN & KIRI) ----
    canvas->setTextSize(1);
    // Sein Kiri
    uint16_t leftArrowColor = (currentSteerState == 1 && blinkState) ? GREEN : DARK_BLUE;
    canvas->fillTriangle(18, 52, 30, 40, 30, 64, leftArrowColor);
    canvas->fillRect(30, 47, 12, 10, leftArrowColor);
    canvas->setTextColor(leftArrowColor == GREEN ? GREEN : GRAY);
    canvas->setCursor(20, 72); canvas->print("LEFT");
    drawSteeringIcon(28, 100);

    // Sein Kanan
    uint16_t rightArrowColor = (currentSteerState == 2 && blinkState) ? GREEN : DARK_BLUE;
    canvas->fillTriangle(302, 52, 290, 40, 290, 64, rightArrowColor);
    canvas->fillRect(278, 47, 12, 10, rightArrowColor);
    canvas->setTextColor(rightArrowColor == GREEN ? GREEN : GRAY);
    canvas->setCursor(278, 72); canvas->print("RIGHT");
    drawSteeringIcon(292, 100);

    // ---- D. LENGKUNGAN BUSUR UTAMA SPEEDOMETER (ARC TINGKAT LINGKARAN) ----
    int arcBlank = 120; 
    int currentArcLength = map(speedValue, 0, 120, 0, 360 - arcBlank);

    for (int r = 73; r > 68; r--) { // Ukuran lingkaran diperbesar agar tidak mencekik teks
        canvas->drawArc(160, 88, r, r - 1, 150, 390, DARK_BLUE); 
        if (currentArcLength > 0) {
            canvas->drawArc(160, 88, r, r - 1, 150, 150 + currentArcLength, CYAN);
        }
    }
    
    // Posisi skala angka kecil disesuaikan di luar busur agar rapi
    canvas->setTextColor(GRAY);
    canvas->setCursor(90, 120); canvas->print("0");
    canvas->setCursor(92, 85);  canvas->print("20");
    canvas->setCursor(112, 50); canvas->print("40");
    canvas->setCursor(154, 34); canvas->print("60"); // Digeser naik ke atas memberi ruang
    canvas->setCursor(196, 50); canvas->print("80");
    canvas->setCursor(216, 85); canvas->print("120");

    // Teks Label "SPEED" dinaikkan ke sumbu Y:45
    canvas->setTextColor(CYAN);
    canvas->setCursor(145, 45);
    canvas->print("SPEED");

    // ---- E. DIGIT ANGKA KECEPATAN BESAR DI TENGAH DASBOR (FIX SIZE 5) ----
    char speedText[4];
    sprintf(speedText, "%03d", speedValue);
    
    canvas->setTextSize(5); // Ukuran 5 sangat aman di kanvas internal, tidak akan corrupt/pecah
    // Efek pendaran teks belakang
    canvas->setTextColor(DARK_BLUE);
    canvas->setCursor(117, 61); canvas->print(speedText);
    canvas->setCursor(119, 63); canvas->print(speedText);
    // Angka Utama Putih Jernih di Tengah
    canvas->setTextColor(WHITE);
    canvas->setCursor(118, 62); 
    canvas->print(speedText);

    // Teks KM/H diturunkan ke bawah sumbu Y:106
    canvas->setTextSize(1);
    canvas->setTextColor(WHITE);
    canvas->setCursor(146, 106);
    canvas->print("KM/H");

    // ---- F. FOOTER BAWAH PANEL STATUS INDIKATOR ----
    // 1. Kotak Informasi Baterai Volt (Kiri Bawah)
    canvas->drawRect(15, 135, 75, 30, DARK_BLUE);
    canvas->setTextColor(GRAY);
    canvas->setCursor(23, 140); canvas->print("BATTERY");
    canvas->setTextColor(WHITE);
    canvas->setCursor(23, 152); canvas->print("12.4V");

    // 2. Kotak Informasi Top Speed Tengah Bawah
    canvas->drawRect(110, 131, 100, 22, DARK_BLUE);
    canvas->setTextColor(CYAN);
    canvas->setCursor(132, 135); canvas->print("TOP SPEED");
    canvas->setTextColor(WHITE);
    canvas->setCursor(136, 144); canvas->printf("%d KM/H", topSpeed);

    // 3. Kotak Informasi Temperatur (Kanan Bawah)
    canvas->drawRect(230, 135, 75, 30, DARK_BLUE);
    canvas->setTextColor(GRAY);
    canvas->setCursor(236, 140); canvas->print("TEMPERATURE");
    drawThermometerIcon(238, 148);
    canvas->setTextColor(WHITE);
    canvas->setCursor(254, 152); canvas->printf("%d 'C", temperature);

    // 4. Bar Garis RPM Warna Warni Horizontal (Paling Bawah)
    canvas->setTextColor(WHITE);
    canvas->setCursor(110, 159); canvas->print("RPM");
    
    int barWidth = map(speedValue, 0, 120, 0, 70);
    canvas->fillRect(135, 161, 70, 4, DARK_BLUE); 
    for (int i = 0; i < barWidth; i++) {
        uint16_t col = GREEN;
        if (i > 35 && i <= 55) col = YELLOW;
        else if (i > 55)       col = RED;
        canvas->drawFastVLine(135 + i, 160, 5, col);
    }

    // ==================================================================
    // KIRIM SELURUH KANVAS MEMORI KE LAYAR FISIK LCD (DOUBLE BUFFERING)
    // ==================================================================
    gfx->draw16bitRGBBitmap(0, 0, canvas->getFramebuffer(), 320, 172);

    delay(5); 
}
