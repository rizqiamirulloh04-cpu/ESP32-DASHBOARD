#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <math.h>

// ======================================================================
// WAVESHARE ESP32-C6 1.47" - CODE V35: SPEED ARC REAL KOMET GRADATION EFFECT
// ======================================================================

#define TFT_BL 22
#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS   14
#define TFT_DC   15
#define TFT_RST  21

// Pin Receiver (DISETEL SEBAGAI INPUT_PULLUP AGAR TIDAK MENGGANGGU BOOT)
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
#define DARK_BLUE      0x0010
#define GRAY           0x5AEB

// INITIALISASI HARDWARE DISPLAY
Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, GFX_NOT_DEFINED);
Arduino_GFX *gfx = new Arduino_ST7789(bus, TFT_RST, 1, true, 172, 320, 34, 0, 34, 0);
Arduino_Canvas *canvas = new Arduino_Canvas(320, 172, gfx);

// VARIABEL GLOBAL (Sama seperti logika Anda)
float smoothedThrottle = 1500.0;
float smoothedSteer = 1500.0;
const float THROTTLE_SMOOTH_FACTOR = 0.45; 
const float STEER_SMOOTH_FACTOR = 0.35;    
float speedValueFiltered = 0.0; 
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
const int centerX = 160; const int centerY = 85;  
const int rx = 86; const int ry = 48;      
const int startAngle = 145; const int endAngle = 395;

// (Fungsi drawCustomOvalArc, printAutoCenterLabel, drawIcon dsb... 
// silakan tempel di sini dari kode Anda sebelumnya agar tidak terpotong)

void setup() {
    // 1. ISOLASI PIN: Set ke INPUT_PULLUP agar tidak "floating" saat start
    pinMode(STEER_PIN, INPUT_PULLUP);
    pinMode(THROTTLE_PIN, INPUT_PULLUP);
    pinMode(BATTERY_PIN, INPUT);
    pinMode(SIGNAL_PIN, INPUT);

    Serial.begin(115200);
    
    // 2. BACKLIGHT
    ledcAttach(TFT_BL, 5000, 8);
    ledcWrite(TFT_BL, 150); 

    // 3. START DISPLAY (LAYAR HARUS JALAN DULU)
    gfx->begin();
    gfx->invertDisplay(false);
    gfx->setRotation(1); 
    canvas->begin();
    canvas->fillScreen(BLACK);

    // 4. JEDA BOOTING (PENTING AGAR PIN STABIL)
    delay(1000); 
}

void loop() {
    // Membaca sinyal receiver
    int rawSteerPWM = pulseIn(STEER_PIN, HIGH, 20000);
    int rawThrottlePWM = pulseIn(THROTTLE_PIN, HIGH, 20000);

    // Proteksi jika receiver tidak terbaca
    if (rawSteerPWM == 0) rawSteerPWM = 1500;
    if (rawThrottlePWM == 0) rawThrottlePWM = 1500; 

    // Logika perhitungan speed, steer, dan drawing canvas Anda 
    // (Masukkan logika loop Anda di bawah ini)
    
    // ... (Logika loop Anda)
    
    gfx->draw16bitRGBBitmap(0, 0, canvas->getFramebuffer(), 320, 172);
    delay(3); 
}
