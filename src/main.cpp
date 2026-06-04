#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// ======================================================================
// WAVESHARE ESP32-C6 1.47" - CODE FIX V5: COMPACT ARC & SPACED TEXTS
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
// FUNGSI GRADASI PIKSEL BUSUR
// ======================================================================
uint16_t getArcGradientColor(int currentAngle, int startAngle, int endAngle) {
    float ratio = (float)(currentAngle - startAngle) / (float)(endAngle - startAngle);
    if (ratio < 0.0) ratio = 0.0;
    if (ratio > 1.0) ratio = 1.0;

    uint8_t startR = 31; 
    uint8_t startG = 26; 
    uint8_t startB = 22; 

    uint8_t endR = 14;  
    uint8_t endG = 0;   
    uint8_t endB = 2;   

    uint8_t r = startR + (endR - startR) * ratio;
    uint8_t g = startG + (endG - startG) * ratio;
    uint8_t b = startB + (endB - startB) * ratio;

    return (r << 11) | (g << 5) | b;
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

    // ---- C. INDIKATOR LAMPU SEIN ----
    canvas->setTextSize(1);
    uint16_t leftArrowColor = (currentSteerState == 1 && blinkState) ? GREEN : DARK_BLUE;
    canvas->fillTriangle(18, 52, 30, 40, 30, 64, leftArrowColor);
    canvas->fillRect(30, 47, 12, 10, leftArrowColor);
    canvas->setTextColor(leftArrowColor == GREEN ? GREEN : GRAY);
    canvas->setCursor(20, 72); canvas->print("LEFT");
    drawSteeringIcon(28, 104);

    uint16_t rightArrowColor = (currentSteerState == 2 && blinkState) ? GREEN : DARK_BLUE;
    canvas->fillTriangle(302, 52, 290, 40, 290, 64, rightArrowColor);
    canvas->fillRect(278, 47, 12, 10, rightArrowColor);
    canvas->setTextColor(rightArrowColor == GREEN ? GREEN : GRAY);
    canvas->setCursor(278, 72); canvas->print("RIGHT");
    drawSteeringIcon(292, 104);

    // ---- D. LENGKUNGAN BUSUR SPEEDOMETER (COMPACT & LOWERED) ----
    // FIX UTAMA: Radius diturunkan ke r=60 agar melengkung di bawah angka 60
    int centerX = 160;
    int centerY = 106; 
    int startAngle = 150;
    int endAngle = 390;
    int totalArcLength = endAngle - startAngle; 
    int currentArcLength = map(speedValue, 0, 120, 0, totalArcLength);

    for (int r = 60; r > 55; r--) { 
        canvas->drawArc(centerX, centerY, r, r - 1, startAngle, endAngle, DARK_BLUE); 
    }

    if (currentArcLength > 0) {
        for (int angle = startAngle; angle < (startAngle + currentArcLength); angle++) {
            uint16_t pixelColor = getArcGradientColor(angle, startAngle, endAngle);
            for (int r = 60; r > 55; r--) {
                canvas->drawArc(centerX, centerY, r, r - 1, angle, angle + 1, pixelColor);
            }
        }
    }
    
    // ---- E. RE-KALIBRASI GRID ANGKA SKALA OUTSIDE ARC ----
    canvas->setTextColor(GRAY);
    canvas->setTextSize(1);
    
    // Angka diposisikan presisi melingkari radius luar busur baru yang lebih kecil
    canvas->setCursor(92, 122);   canvas->print("0");    // Aman di atas kotak BATTERY
    canvas->setCursor(84, 90);    canvas->print("20");   
    canvas->setCursor(102, 58);   canvas->print("40");   
    canvas->setCursor(154, 26);   canvas->print("60");   // FIX: Berada di atas busur, di bawah SPORT MODE
    canvas->setCursor(208, 58);   canvas->print("80");   
    canvas->setCursor(224, 90);   canvas->print("100");  
    canvas->setCursor(218, 122);  canvas->print("120");  // Aman di atas kotak TEMPERATURE

    // Label SPEED diposisikan manis di dalam busur
    canvas->setTextColor(CYAN);
    canvas->setCursor(145, 50);
    canvas->print("SPEED");

    // ---- F. DIGIT KECEPATAN UTAMA DI TENGAH (SIZE 4) ----
    char speedText[4];
    sprintf(speedText, "%03d", speedValue);
    
    canvas->setTextSize(4); 
    // Shadow belakang
    canvas->setTextColor(DARK_BLUE);
    canvas->setCursor(123, 67); canvas->print(speedText);
    canvas->setCursor(125, 69); canvas->print(speedText);
    // Angka utama
    canvas->setTextColor(WHITE);
    canvas->setCursor(124, 68); 
    canvas->print(speedText);

    // Teks KM/H di bawah angka utama
    canvas->setTextSize(1);
    canvas->setTextColor(WHITE);
    canvas->setCursor(146, 105);
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

    // 4. Bar Garis RPM Warna Warni Horizontal (Paling Bawah)
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
