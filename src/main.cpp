#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <math.h>

// ======================================================================
// WAVESHARE ESP32-C6 1.47" - CODE V28: EFEK KOMET BERBUNTUT PADA BAR RPM
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
#define RED_DARK       0x8000 
#define RED_BRIGHT     0xF800 
#define PINK_LIGHT     0xFDB8 
#define GREEN_BRIGHT   0x07E0 // Kepala Komet (Hijau Muda Terang)
#define YELLOW         0xFFE0
#define CYAN           0x07FF
#define DARK_BLUE      0x0010 
#define GRAY           0x5AEB

// INITIALISASI HARDWARE DISPLAY
Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, GFX_NOT_DEFINED);
Arduino_GFX *gfx = new Arduino_ST7789(bus, TFT_RST, 1, true, 172, 320, 34, 0, 34, 0);

// Canvas Utama Ukuran Penuh (320x172)
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

// CONFIG ELIPS PRESISI
const int centerX = 160;
const int centerY = 85;  
const int rx = 86;       
const int ry = 48;       

const int startAngle = 145;
const int endAngle = 395;

// ======================================================================
// FUNGSI UTAMA: MENGGAMBAR BUSUR OVAL KECEPATAN
// ======================================================================
void drawCustomOvalArc(int cx, int cy, int rx, int ry, int startDeg, int endDeg, uint16_t defaultColor, int thickness, bool drawTicks, bool isSpeedArc) {
    for (int t = 0; t < thickness; t++) {
        int curRx = rx - t;
        int curRy = ry - t;
        
        for (int angle = startDeg; angle <= endDeg; angle++) {
            float rad = (float)angle * M_PI / 180.0;
            int x = cx + (int)(cos(rad) * curRx);
            int y = cy + (int)(sin(rad) * curRy);
            
            uint16_t pixelColor = defaultColor;

            if (isSpeedArc) {
                int progress = angle - startAngle;
                int totalRange = endAngle - startAngle;
                
                if (progress < (totalRange / 3)) {
                    pixelColor = PINK_LIGHT; 
                } else if (progress < (2 * totalRange / 3)) {
                    pixelColor = RED_BRIGHT;
                } else {
                    pixelColor = RED_DARK;
                }
            }
            
            if (x >= 0 && x < 320 && y >= 0 && y < 172) {
                canvas->drawPixel(x, y, pixelColor);
            }

            if (drawTicks && t == 0 && (angle % 4 == 0)) {
                for (int tickLen = 1; tickLen <= 4; tickLen++) {
                    int tx = cx + (int)(cos(rad) * (rx + tickLen));
                    int ty = cy + (int)(sin(rad) * (ry + tickLen));
                    if (tx >= 0 && tx < 320 && ty >= 0 && ty < 172) {
                        canvas->drawPixel(tx, ty, GRAY); 
                    }
                }
            }
        }
    }
}

// ======================================================================
// FUNGSI PRINT ANGKA OTOMATIS MENGIKUTI POSISI DERAJAT ELIPS
// ======================================================================
void printAutoCenterLabel(const char* label, int angle, int textGap) {
    float rad = (float)angle * M_PI / 180.0;
    int targetX = centerX + (int)(cos(rad) * (rx + textGap));
    int targetY = centerY + (int)(sin(rad) * (ry + textGap));
    int stringWidth = strlen(label) * 6; 
    int stringHeight = 8;
    canvas->setCursor(targetX - (stringWidth / 2), targetY - (stringHeight / 2));
    canvas->print(label);
}

// ======================================================================
// FUNGSI GRAFIS IKON STATUS
// ======================================================================
void drawSignalIcon(int x, int y) {
    canvas->fillCircle(x + 10, y + 12, 2, CYAN);
    canvas->drawArc(x + 10, y + 12, 5, 4, 220, 320, CYAN);
    canvas->drawArc(x + 10, y + 12, 9, 8, 220, 320, CYAN);
}

void drawBatteryIcon(int x, int y) {
    canvas->drawRect(x, y + 3, 18, 10, GRAY);
    canvas->fillRect(x + 18, y + 6, 2, 4, GRAY);
    canvas->fillRect(x + 2, y + 5, 14, 6, GREEN_BRIGHT);
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
    canvas->fillRect(x + 4, y + 3, 1, 10, RED_BRIGHT);
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

    canvas->fillScreen(BLACK); 
    canvas->setFont(NULL); 

    // ---- A. GARIS HEADER PANEL ATAS ----
    canvas->drawLine(10, 14, 310, 14, CYAN); 

    // ---- B. INFO STATUS HEADER ATAS ----
    drawSignalIcon(15, 1);
    canvas->setTextColor(WHITE);
    canvas->setCursor(40, 4);
    canvas->printf("%d dBm", signalDbm);
    
    drawBatteryIcon(250, 1);
    canvas->setCursor(275, 4);
    canvas->printf("%d%%", batteryPercent);

    // ---- C. LAMPU SEIN KIRI & KANAN ----
    uint16_t leftArrowColor = (currentSteerState == 1 && blinkState) ? GREEN_BRIGHT : DARK_BLUE;
    canvas->fillTriangle(18, 52, 30, 40, 30, 64, leftArrowColor);
    canvas->fillRect(30, 47, 12, 10, leftArrowColor);
    canvas->setTextColor(leftArrowColor == GREEN_BRIGHT ? GREEN_BRIGHT : GRAY);
    canvas->setCursor(20, 72); canvas->print("LEFT");
    drawSteeringIcon(28, 102);

    uint16_t rightArrowColor = (currentSteerState == 2 && blinkState) ? GREEN_BRIGHT : DARK_BLUE;
    canvas->fillTriangle(302, 52, 290, 40, 290, 64, rightArrowColor);
    canvas->fillRect(278, 47, 12, 10, rightArrowColor);
    canvas->setTextColor(rightArrowColor == GREEN_BRIGHT ? GREEN_BRIGHT : GRAY);
    canvas->setCursor(278, 72); canvas->print("RIGHT");
    drawSteeringIcon(292, 102);

    // ---- D. RENDERING BUSUR ELIPS OVAL ----
    int currentActiveAngle = map(speedValue, 0, 120, startAngle, endAngle);
    
    // Background busur asli Biru Gelap (DARK_BLUE)
    drawCustomOvalArc(centerX, centerY, rx, ry, startAngle, endAngle, DARK_BLUE, 3, true, false);
    
    // Sinar merah bergerak aktif
    if (speedValue > 0) {
        drawCustomOvalArc(centerX, centerY, rx, ry, startAngle, currentActiveAngle, BLACK, 3, false, true);
    }
    
    // ---- E. DISTRIBUSI LABEL ANGKA KELILING ----
    canvas->setTextColor(GRAY);
    canvas->setTextSize(1);
    
    printAutoCenterLabel("20",  182, 13); 
    printAutoCenterLabel("40",  215, 13); 
    printAutoCenterLabel("60",  270, 10); 
    printAutoCenterLabel("80",  325, 13); 
    printAutoCenterLabel("100", 358, 13); 

    canvas->setCursor(68, 122);  canvas->print("0");   
    canvas->setCursor(243, 122); canvas->print("120"); 

    // ---- F. CLUSTER TENGAH ----
    canvas->setTextSize(1);
    canvas->setTextColor(CYAN);
    int speedX = 145; int speedY = 56;  
    canvas->setCursor(speedX, speedY);     canvas->print("SPEED");
    canvas->setCursor(speedX + 1, speedY); canvas->print("SPEED"); 

    char speedText[4];
    sprintf(speedText, "%03d", speedValue);
    canvas->setTextSize(4); canvas->setTextColor(WHITE);
    int textX = 122; int textY = 69;  
    canvas->setCursor(textX, textY);         canvas->print(speedText);
    canvas->setCursor(textX + 1, textY);     canvas->print(speedText);
    canvas->setCursor(textX - 1, textY);     canvas->print(speedText);
    canvas->setCursor(textX, textY + 1);     canvas->print(speedText);
    canvas->setCursor(textX, textY - 1);     canvas->print(speedText);

    canvas->setTextSize(1); canvas->setTextColor(WHITE);
    int kmhX = 148; int kmhY = 105;  
    canvas->setCursor(kmhX, kmhY);     canvas->print("KM/H");
    canvas->setCursor(kmhX + 1, kmhY); canvas->print("KM/H"); 


    // ---- G. FOOTER PANEL STATUS INDIKATOR BAWAH ----
    canvas->drawRect(15, 142, 75, 26, DARK_BLUE);
    canvas->setTextColor(GRAY); canvas->setTextSize(1);
    canvas->setCursor(23, 145); canvas->print("BATTERY");
    canvas->setTextColor(WHITE); canvas->setCursor(23, 156); canvas->print("12.4V");

    canvas->drawRect(110, 137, 100, 19, DARK_BLUE);
    canvas->setTextColor(CYAN); canvas->setCursor(132, 140); canvas->print("TOP SPEED");
    canvas->setTextColor(WHITE); canvas->setCursor(136, 147); canvas->printf("%d KM/H", topSpeed);

    canvas->drawRect(230, 142, 75, 26, DARK_BLUE);
    canvas->setTextColor(GRAY); canvas->setCursor(236, 145); canvas->print("TEMPERATURE");
    drawThermometerIcon(238, 151);
    canvas->setTextColor(WHITE); canvas->setCursor(254, 156); canvas->printf("%d 'C", temperature);

    // ---- H. BAR RPM EFEK KOMET BERBUNTUT (FADING GREEN GRADIENT) ----
    canvas->setTextColor(WHITE);
    canvas->setCursor(110, 163); canvas->print("RPM");
    
    canvas->fillRect(135, 165, 70, 4, DARK_BLUE); // Background Rel/Jalur Bar Kosong
    int barWidth = map(speedValue, 0, 120, 0, 70);
    
    for (int i = 0; i < barWidth; i++) {
        uint16_t col = GREEN_BRIGHT; // Default Kepala Komet (Terang Benderang)
        
        // Logika Fading (Buntut Komet memudar ke belakang)
        int distanceFromHead = barWidth - 1 - i; 
        
        if (distanceFromHead > 0) {
            // Menggeser kecerahan komponen Hijau (Green dalam RGB565 berada di bit tengah)
            // Semakin jauh dari kepala komet, warna hijaunya semakin diredupkan
            int intensity = 63 - (distanceFromHead * 4); 
            if (intensity < 12) intensity = 12; // Batas minimal redup agar buntut tidak hilang total
            col = (intensity << 5); // Terapkan nilai intensitas ke bit warna hijau
        }
        
        canvas->drawFastVLine(135 + i, 164, 5, col);
    }

    // TAMPILKAN SELURUH MEMORI KANVAS KE LCD
    gfx->draw16bitRGBBitmap(0, 0, canvas->getFramebuffer(), 320, 172);

    delay(5); 
}
