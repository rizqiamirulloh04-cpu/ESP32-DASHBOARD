#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <math.h>

// ======================================================================
// WAVESHARE ESP32-C6 1.47" - CODE FIX V7: PERFECT OVAL RADIAL DASHBOARD
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
#define DARK_BLUE      0x0010 // Biru redup background
#define GRAY           0x5AEB

// INITIALISASI HARDWARE DISPLAY
Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, GFX_NOT_DEFINED);
Arduino_GFX *gfx = new Arduino_ST7789(bus, TFT_RST, 1, true, 172, 320, 34, 0, 34, 0);

// Canvas Utama Ukuran Penuh (320x172) untuk Double Buffering
Arduino_Canvas *canvas = new Arduino_Canvas(320, 172, gfx);

int speedValue = 0; 
int targetSpeed = 0;
int topSpeed = 0;
bool blinkState = false;
unsigned long blinkTimer = 0;
int currentSteerState = 0;

int batteryPercent = 89;
int signalDbm = -67;
int temperature = 38;
unsigned long sensorTimer = 0;

// ======================================================================
// FUNGSI GRADASI WARNA BUSUR OVAL
// ======================================================================
uint16_t getOvalGradientColor(int currentAngle, int startAngle, int endAngle) {
    float ratio = (float)(currentAngle - startAngle) / (float)(endAngle - startAngle);
    if (ratio < 0.0) ratio = 0.0;
    if (ratio > 1.0) ratio = 1.0;

    // Gradasi dari Biru Muda (Cyan) ke Merah di ujung kanan skala
    uint8_t startR = 0;   uint8_t startG = 28;  uint8_t startB = 31; // CYAN
    uint8_t endR = 31;    uint8_t endG = 0;     uint8_t endB = 0;  // RED

    uint8_t r = startR + (endR - startR) * ratio;
    uint8_t g = startG + (endG - startG) * ratio;
    uint8_t b = startB + (endB - startB) * ratio;

    return (r << 11) | (g << 5) | b;
}

// ======================================================================
// FUNGSI CUSTOM UNTUK MENGGAMBAR BUSUR OVAL (PIPIH) PERSIS SEPERTI GAMBAR
// ======================================================================
void drawCustomOvalArc(int cx, int cy, int rx, int ry, int startDeg, int endDeg, uint16_t color, int thickness) {
    // Loop untuk menggambar ketebalan garis busur ke arah dalam
    for (int t = 0; t < thickness; t++) {
        int curRx = rx - t;
        int curRy = ry - t;
        
        // Gambar pixel demi pixel berdasarkan sudut derajat matematika
        for (int angle = startDeg; angle <= endDeg; angle++) {
            float rad = (float)angle * M_PI / 180.0;
            // Gunakan rumus koordinat elips/oval
            int x = cx + (int)(cos(rad) * curRx);
            int y = cy + (int)(sin(rad) * curRy);
            
            // Proteksi batas layar agar tidak crash
            if (x >= 0 && x < 320 && y >= 0 && y < 172) {
                // Jika warna adalah WHITE (berarti mode render bar aktif), pakai warna gradasi dinamis
                if (color == WHITE) {
                    uint16_t dynamicColor = getOvalGradientColor(angle, 145, 395);
                    canvas->drawPixel(x, y, dynamicColor);
                } else {
                    canvas->drawPixel(x, y, color);
                }
            }
        }
    }
}

// ======================================================================
// FUNGSI GRAFIS IKON
// ======================================================================
void drawSignalIcon(int x, int y) {
    canvas->fillCircle(x + 10, y + 12, 2, CYAN);
    canvas->drawArc(x + 10, y + 12, 5, 4, 220, 320, CYAN);
    canvas->drawArc(x + 10, y + 12, 9, 8, 220, 320, CYAN);
}

void drawBatteryIcon(int x, int y) {
    canvas->drawRect(x, y + 3, 18, 10, GRAY);
    canvas->fillRect(x + 18, y + 6, 2, 4, GRAY);
    canvas->fillRect(x + 2, y + 5, 14, 6, GREEN);
}

void drawSteeringIcon(int x, int y) {
    canvas->drawCircle(x, y, 12, GRAY);
    canvas->drawCircle(x, y, 2, GRAY);
    canvas->drawLine(x - 11, y, x + 11, y, GRAY);
    canvas->drawLine(x, y + 2, x, y + 11, GRAY);
}

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
    ledcWrite(TFT_BL, 150); 

    gfx->begin();
    gfx->invertDisplay(false);
    gfx->setRotation(1); 

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
    int steerPWM = pulseIn(STEER_PIN, HIGH, 10000);
    int throttlePWM = pulseIn(THROTTLE_PIN, HIGH, 10000);

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

    speedValue = targetSpeed; 

    if (speedValue > topSpeed) topSpeed = speedValue;

    if (millis() - blinkTimer > 350) {
        blinkTimer = millis();
        blinkState = !blinkState;
    }

    currentSteerState = 0; 
    if (steerPWM < 1400)      currentSteerState = 1; 
    else if (steerPWM > 1600) currentSteerState = 2; 

    if (millis() - sensorTimer > 500) {
        sensorTimer = millis();
        int rawBat = analogRead(BATTERY_PIN);
        batteryPercent = map(rawBat, 0, 4095, 0, 100);
        batteryPercent = constrain(batteryPercent, 0, 100);
    }

    // ==================================================================
    // RENDER GRAFIS
    // ==================================================================
    canvas->fillScreen(BLACK); 
    canvas->setFont(NULL); 

    // ---- A. GARIS DEKORASI HEADER ATAS ----
    canvas->setTextColor(CYAN);
    canvas->setTextSize(1);
    canvas->setCursor(132, 2); 
    canvas->print("SPORT MODE");

    canvas->drawLine(10, 14, 115, 14, CYAN);
    canvas->drawLine(115, 14, 123, 5, CYAN);
    canvas->drawLine(123, 5, 197, 5, CYAN);
    canvas->drawLine(197, 5, 205, 14, CYAN);
    canvas->drawLine(205, 14, 310, 14, CYAN);

    // ---- B. INFO SINYAL & BATERAI ATAS ----
    drawSignalIcon(15, 1);
    canvas->setTextColor(WHITE);
    canvas->setCursor(40, 4);
    canvas->printf("%d dBm", signalDbm);
    
    drawBatteryIcon(250, 1);
    canvas->setCursor(275, 4);
    canvas->printf("%d%%", batteryPercent);

    // ---- C. INDIKATOR LAMPU SEIN KIRI & KANAN ----
    canvas->setTextSize(1);
    uint16_t leftArrowColor = (currentSteerState == 1 && blinkState) ? GREEN : DARK_BLUE;
    canvas->fillTriangle(18, 52, 30, 40, 30, 64, leftArrowColor);
    canvas->fillRect(30, 47, 12, 10, leftArrowColor);
    canvas->setTextColor(leftArrowColor == GREEN ? GREEN : GRAY);
    canvas->setCursor(20, 72); canvas->print("LEFT");
    drawSteeringIcon(28, 102);

    uint16_t rightArrowColor = (currentSteerState == 2 && blinkState) ? GREEN : DARK_BLUE;
    canvas->fillTriangle(302, 52, 290, 40, 290, 64, rightArrowColor);
    canvas->fillRect(278, 47, 12, 10, rightArrowColor);
    canvas->setTextColor(rightArrowColor == GREEN ? GREEN : GRAY);
    canvas->setCursor(278, 72); canvas->print("RIGHT");
    drawSteeringIcon(292, 102);

    // ---- D. FORMULA UTAMA: OVAL RADIAL BUSUR (PERSIS GAMBAR REFERENSI) ----
    // Titik pusat digeser ke bawah (Y=116), Lebar X=88 (Sangat Melebar), Tinggi Y=66 (Pipih ke bawah)
    int centerX = 160;
    int centerY = 116; 
    int rx = 88;
    int ry = 66;
    
    // Sudut derajat elips (145' kiri bawah melingkar atas sampai 395' kanan bawah)
    int startAngle = 145;
    int endAngle = 395;
    int currentActiveAngle = map(speedValue, 0, 120, startAngle, endAngle);

    // 1. Gambar Background Busur Oval (Biru Redup) dengan ketebalan 4 pixel
    drawCustomOvalArc(centerX, centerY, rx, ry, startAngle, endAngle, DARK_BLUE, 4);

    // 2. Gambar Lapisan Bar Aktif Kecepatan (Gradasi Berwarna) di atas background
    if (speedValue > 0) {
        drawCustomOvalArc(centerX, centerY, rx, ry, startAngle, currentActiveAngle, WHITE, 4);
    }
    
    // ---- E. RE-POSITIONING GRID ANGKA SKALA OUTSIDE OVAL ----
    // Angka diposisikan presisi melingkari kontur luar elips/oval secara estetis
    canvas->setTextColor(GRAY);
    canvas->setTextSize(1);
    
    canvas->setCursor(55, 120);   canvas->print("0");    // Kiri bawah dekat Battery
    canvas->setCursor(56, 82);    canvas->print("20");   // Sisi kiri luar elips
    canvas->setCursor(92, 44);    canvas->print("40");   // Naik ke atas kiri
    canvas->setCursor(152, 24);   canvas->print("60");   // FIX UTAMA: Tepat di tengah atas, luar busur, di bawah SPORT MODE!
    canvas->setCursor(214, 44);   canvas->print("80");   // Turun ke kanan atas
    canvas->setCursor(248, 82);   canvas->print("100");  // Sisi kanan luar elips
    canvas->setCursor(244, 120);  canvas->print("120");  // Kanan bawah dekat Temp

    // Label teks SPEED pas di dalam puncak elips bawah angka 60
    canvas->setTextColor(CYAN);
    canvas->setCursor(146, 42);
    canvas->print("SPEED");

    // ---- F. DIGIT KECEPATAN UTAMA DI TENGAH (UKURAN BESAR LEGA) ----
    char speedText[4];
    sprintf(speedText, "%03d", speedValue);
    
    canvas->setTextSize(4); 
    // Efek Shadow belakang angka utama
    canvas->setTextColor(DARK_BLUE);
    canvas->setCursor(123, 62); canvas->print(speedText);
    canvas->setCursor(125, 64); canvas->print(speedText);
    // Angka utama putih solid di tengah elips
    canvas->setTextColor(WHITE);
    canvas->setCursor(124, 63); 
    canvas->print(speedText);

    // Teks KM/H tepat di bawah digit besar
    canvas->setTextSize(1);
    canvas->setTextColor(WHITE);
    canvas->setCursor(146, 100);
    canvas->print("KM/H");

    // ---- G. FOOTER PANEL STATUS INDIKATOR BAWAH ----
    // 1. Kotak Informasi Baterai Volt (Kiri Bawah)
    canvas->drawRect(15, 137, 75, 30, DARK_BLUE);
    canvas->setTextColor(GRAY);
    canvas->setCursor(23, 142); canvas->print("BATTERY");
    canvas->setTextColor(WHITE);
    canvas->setCursor(23, 154); canvas->print("12.4V");

    // 2. Kotak Informasi Top Speed (Tengah Bawah)
    canvas->drawRect(110, 133, 100, 22, DARK_BLUE);
    canvas->setTextColor(CYAN);
    canvas->setCursor(132, 137); canvas->print("TOP SPEED");
    canvas->setTextColor(WHITE);
    canvas->setCursor(136, 146); canvas->printf("%d KM/H", topSpeed);

    // 3. Kotak Informasi Temperatur (Kanan Bawah)
    canvas->drawRect(230, 137, 75, 30, DARK_BLUE);
    canvas->setTextColor(GRAY);
    canvas->setCursor(236, 142); canvas->print("TEMPERATURE");
    drawThermometerIcon(238, 150);
    canvas->setTextColor(WHITE);
    canvas->setCursor(254, 154); canvas->printf("%d 'C", temperature);

    // 4. Bar Garis RPM Horizontal (Paling Bawah)
    canvas->setTextColor(WHITE);
    canvas->setCursor(110, 161); canvas->print("RPM");
    
    int barWidth = map(speedValue, 0, 120, 0, 70);
    canvas->fillRect(135, 163, 70, 4, DARK_BLUE); 
    for (int i = 0; i < barWidth; i++) {
        uint16_t col = GREEN;
        if (i > 35 && i <= 55) col = YELLOW;
        else if (i > 55)       col = RED;
        canvas->drawFastVLine(135 + i, 162, 5, col);
    }

    // KIRIM MEMORI KANVAS KE FISIK LCD
    gfx->draw16bitRGBBitmap(0, 0, canvas->getFramebuffer(), 320, 172);

    delay(5); 
}
