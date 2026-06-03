#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// ======================================================
// WAVESHARE ESP32-C6 1.47"
// RACING DASHBOARD WITH BACKLIGHT GREEN GLOW EFFECT
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

// ================= COLORS =================
#define BLACK         0x0000
#define WHITE         0xFFFF
#define RED           0xF800
#define GREEN         0x07E0
#define BLUE          0x001F
#define CYAN          0x07FF
#define YELLOW        0xFFE0
#define ICE           0xCE79

// Warna Gradasi untuk Efek Neon Glow Hijau (Makin besar angkanya, makin gelap)
#define GLOW_GREEN_1  0x0204 // Hijau sangat redup (luar)
#define GLOW_GREEN_2  0x0306 // Hijau redup sedang
#define GLOW_GREEN_3  0x0408 // Hijau agak terang (pusat)
#define GLOW_LINE_CLR 0x1208 // Garis pembatas gelap

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
// FUNCTION FOR BACKLIGHT GLOW EFFECT (Efek Pendaran Lampu)
// ======================================================
void drawGlowBackground()
{
    // Menggambar lingkaran berlapis dari luar ke dalam untuk membentuk gradasi lampu halus
    // Pusat lingkaran diatur di tengah layar (X=160, Y=80)
    for (int r = 85; r > 0; r -= 5) 
    {
        uint16_t glowColor = BLACK;
        if (r > 60)       glowColor = GLOW_GREEN_1; // Lapisan terluar (paling redup)
        else if (r > 35)  glowColor = GLOW_GREEN_2; // Lapisan tengah
        else              glowColor = GLOW_GREEN_3; // Lapisan pusat (paling terang)
        
        gfx->fillCircle(160, 80, r, glowColor);
    }
}

// ======================================================
// STATIC UI (Digambar sekali di awal)
// ======================================================
void drawStaticUI()
{
    gfx->fillScreen(BLACK);

    // 1. Gambar efek pendaran lampu hijau di latar belakang terlebih dahulu
    drawGlowBackground();

    // 2. KM/H TEXT
    gfx->setTextColor(CYAN);
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

    // ==================================================
    // BACKLIGHT (Kecerahan 50%)
    // ==================================================
    ledcAttach(TFT_BL, 5000, 8);
    ledcWrite(TFT_BL, 128); 

    // ==================================================
    // DISPLAY INITIALIZATION
    // ==================================================
    gfx->begin();
    gfx->invertDisplay(false);
    gfx->setRotation(1); 

    drawStaticUI();

    // ==================================================
    // INPUT
    // ==================================================
    pinMode(STEER_PIN, INPUT);
    pinMode(THROTTLE_PIN, INPUT);
}

// ======================================================
// LOOP
// ======================================================
void loop()
{
    // ==================================================
    // READ PWM
    // ==================================================
    int steerPWM = pulseIn(STEER_PIN, HIGH, 25000);
    int throttlePWM = pulseIn(THROTTLE_PIN, HIGH, 25000);

    // FAILSAFE
    if (steerPWM == 0)     steerPWM = 1500;
    if (throttlePWM == 0)  throttlePWM = 1500; 

    // ==================================================
    // LOGIKA FILTER GAS MAJU / MUNDUR
    // ==================================================
    if (throttlePWM < 1490) 
    {
        targetSpeed = map(throttlePWM, 1500, 1000, 0, 120);
        targetSpeed = constrain(targetSpeed, 0, 120);
    } 
    else if (throttlePWM > 1510) 
    {
        targetSpeed = map(throttlePWM, 1500, 2000, 0, 50);
        targetSpeed = constrain(targetSpeed, 0, 50);
    } 
    else 
    {
        targetSpeed = 0;
    }

    // SMOOTHING DIPERCEPAT
    if (speedValue < targetSpeed) {
        speedValue += 4; 
        if (speedValue > targetSpeed) speedValue = targetSpeed;
    }
    if (speedValue > targetSpeed) {
        speedValue -= 4;
        if (speedValue < targetSpeed) speedValue = targetSpeed;
    }

    // TIMER KEDIP SEIN
    if (millis() - blinkTimer > 350)
    {
        blinkTimer = millis();
        blinkState = !blinkState;
    }

    // ==================================================
    // REFRESH AREA SEIN (Tetap menggunakan BLACK karena di luar area glow)
    // ==================================================
    gfx->fillRect(15, 50, 45, 55, BLACK);  
    gfx->fillRect(260, 50, 50, 55, BLACK); 

    // ==================================================
    // SPEED NUMBER RENDERING WITH GLOW BACKGROUND
    // Trik: Set background text ke warna GLOW_GREEN_3 (warna pusat glow)
    // agar angka menimpa latar belakang pendaran secara mulus tanpa kotak hitam kaku
    // ==================================================
    gfx->setTextColor(ICE, GLOW_GREEN_3);
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
        if (i < 40)       segmentColor = GREEN;   
        else if (i < 80)  segmentColor = YELLOW;  
        else              segmentColor = RED;     
        
        gfx->drawFastVLine(100 + i, 112, 4, segmentColor);
    }

    // Trik: Menghapus sisa bar menggunakan warna GLOW_GREEN_2 karena posisinya ada di area glow bawah
    if (barWidth < 120) {
        gfx->fillRect(100 + barWidth, 112, 120 - barWidth, 4, GLOW_GREEN_2);
    }

    // ==================================================
    // LEFT SIGNAL
    // ==================================================
    if (steerPWM < 1400 && blinkState)
    {
        gfx->fillTriangle(25, 75, 55, 55, 55, 95, YELLOW);
    }

    // ==================================================
    // RIGHT SIGNAL
    // ==================================================
    if (steerPWM > 1600 && blinkState)
    {
        gfx->fillTriangle(295, 75, 265, 55, 265, 95, YELLOW);
    }

    delay(15);
}
