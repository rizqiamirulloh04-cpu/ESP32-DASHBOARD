#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <math.h>
#include <Preferences.h> // Library untuk menyimpan data permanen

// ======================================================================
// WAVESHARE ESP32-C6 1.47" - CODE V56 FIXED: SPEED DELAY & RPM INSTAN
// ======================================================================

Preferences prefs; // Objek untuk NVS

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
#define RED_BRIGHT     0xF800 
#define GREEN_BRIGHT   0x07E0 
#define YELLOW         0xFFE0
#define CYAN           0x07FF 
#define BRIGHT_BLUE    0x03BF 
#define DARK_BLUE      0x0010 
#define GRAY           0x5AEB

Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, GFX_NOT_DEFINED);
Arduino_GFX *gfx = new Arduino_ST7789(bus, TFT_RST, 1, true, 172, 320, 34, 0, 34, 0);
Arduino_Canvas *canvas = new Arduino_Canvas(320, 172, gfx);

float smoothedThrottleSpeed = 1500.0;
float smoothedSteer = 1500.0;
const float SPEED_SMOOTH_FACTOR = 0.18;
const float STEER_SMOOTH_FACTOR = 0.35;    

float speedValueFiltered = 0.0; 
int speedValue = 0; 
int targetSpeed = 0;
int topSpeed = 0; // Nilai ini akan dimuat dari NVS

int rpmBarWidth = 0; 
bool blinkState = false;
unsigned long blinkTimer = 0;
int currentSteerState = 0;
int batteryPercent = 89;
int signalDbm = -67;
int temperature = 38;
unsigned long sensorTimer = 0;

const int centerX = 160;
const int centerY = 85;  
const int rx = 86;       
const int ry = 48;       
const int startAngle = 145;
const int endAngle = 395;

// (Fungsi-fungsi drawCustomOvalArc, printAutoCenterLabel, dll tetap sama di sini)
void drawCustomOvalArc(int cx, int cy, int rx, int ry, int startDeg, int endDeg, uint16_t defaultColor, int thickness, bool drawTicks, bool isSpeedArc) {
    int totalAngles = endDeg - startDeg;
    for (int t = 0; t < thickness; t++) {
        int curRx = rx - t; int curRy = ry - t;
        int step = isSpeedArc ? 1 : 2; 
        for (int angle = startDeg; angle <= endDeg; angle += step) {
            float rad = (float)angle * M_PI / 180.0;
            int x = cx + (int)(cos(rad) * curRx);
            int y = cy + (int)(sin(rad) * curRy);
            uint16_t pixelColor = defaultColor;
            if (isSpeedArc && totalAngles > 0) {
                int currentPos = angle - startDeg; 
                if (currentPos >= totalAngles - 4) pixelColor = WHITE; 
                else {
                    int redIntensity = map(currentPos, 0, totalAngles, 6, 31);
                    redIntensity = constrain(redIntensity, 6, 31);
                    pixelColor = (redIntensity << 11); 
                }
            }
            if (x >= 0 && x < 320 && y >= 0 && y < 172) canvas->drawPixel(x, y, pixelColor);
            if (drawTicks && t == 0 && (angle % 4 == 0)) {
                for (int tickLen = 1; tickLen <= 4; tickLen++) {
                    int tx = cx + (int)(cos(rad) * (rx + tickLen));
                    int ty = cy + (int)(sin(rad) * (ry + tickLen));
                    if (tx >= 0 && tx < 320 && ty >= 0 && ty < 172) canvas->drawPixel(tx, ty, GRAY); 
                }
            }
        }
    }
}

// ... (tambahkan kembali fungsi printAutoCenterLabel, drawSignalIcon, dll di sini agar rapi) ...

void setup() {
    Serial.begin(115200);

    // Inisialisasi NVS
    prefs.begin("speedo", false);
    topSpeed = prefs.getInt("topSpeed", 0); // Muat topSpeed dari memori

    ledcAttach(TFT_BL, 5000, 8);
    ledcWrite(TFT_BL, 100); 
    gfx->begin();
    gfx->setRotation(1); 
    canvas->begin();
    canvas->fillScreen(BLACK);
    pinMode(STEER_PIN, INPUT);
    pinMode(THROTTLE_PIN, INPUT);
}

void loop() {
    // ... (Logika sensor PWM tetap sama) ...
    
    // Logika Auto-Save Top Speed
    if (speedValue > topSpeed) {
        topSpeed = speedValue;
        prefs.putInt("topSpeed", topSpeed); // Simpan ke memori setiap kali memecahkan rekor
    }

    // ... (Sisa rendering canvas dan drawing lainnya tetap sama) ...
    
    gfx->draw16bitRGBBitmap(0, 0, canvas->getFramebuffer(), 320, 172);
    delay(3); 
}
