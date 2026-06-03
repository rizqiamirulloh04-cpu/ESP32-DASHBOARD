#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// ======================================================
// WAVESHARE ESP32-C6 1.47"
// RE-FIXED RESPONSIVE RACING DASHBOARD (ANTI-FLICKER)
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
#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
#define CYAN    0x07FF
#define YELLOW  0xFFE0
#define DARK    0x2104
#define ICE     0xCE79

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
int speedValue = 0; // Mulai dari 0 (Aman dari bug locking)
int targetSpeed = 0;

bool blinkState = false;
unsigned long blinkTimer = 0;

// ======================================================
// STATIC UI (Digambar sekali di awal)
// ======================================================
void drawStaticUI()
{
    gfx->fillScreen(BLACK);

    // KM/H TEXT
    gfx->setTextColor(CYAN);
    gfx->setTextSize(2);
    gfx->setCursor(136, 120); 
    gfx->print("KM/H");

    // GLOW LINE
    gfx->drawFastHLine(110, 110, 100, DARK);
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
    // READ PWM (Deteksi langsung dari Receiver)
    // ==================================================
    int steerPWM = pulseIn(STEER_PIN, HIGH, 25000);
    int throttlePWM = pulseIn(THROTTLE_PIN, HIGH, 25000);

    // FAILSAFE (Jika kabel lepas/sinyal hilang)
    if (steerPWM == 0)     steerPWM = 1500;
    if (throttlePWM == 0)  throttlePWM = 1500; 

    // ==================================================
    // LOGIKA FILTER GAS MAJU / MUNDUR (Lebih Luas & Stabil)
    // ==================================================
    if (throttlePWM < 1490) 
    {
        // MAJU: Sinyal mengecil dari 1500 ke 1000 (0-120 km)
        targetSpeed = map(throttlePWM, 1500, 1000, 0, 120);
        targetSpeed = constrain(targetSpeed, 0, 120);
    } 
    else if (throttlePWM > 1510) 
    {
        // MUNDUR: Sinyal membesar dari 1500 ke 2000 (0-50 km)
        targetSpeed = map(throttlePWM, 1500, 2000, 0, 50);
        targetSpeed = constrain(targetSpeed, 0, 50);
    } 
    else 
    {
        // NETRAL PAS DI TENGAH
        targetSpeed = 0;
    }

    // SMOOTHING (Pergerakan angka naik turun)
    if (speedValue < targetSpeed) speedValue++;
    if (speedValue > targetSpeed) speedValue--;

    // TIMER KEDIP SEIN
    if (millis() - blinkTimer > 350)
    {
        blinkTimer = millis();
        blinkState = !blinkState;
    }

    // ==================================================
    // REFRESH AREA SEIN (Hapus kotak hitam tipis sebelum digambar)
    // Ditulis langsung tanpa 'if' penahan agar responsif instan
    // ==================================================
    gfx->fillRect(15, 50, 45, 55, BLACK);  // Hapus area kiri
    gfx->fillRect(260, 50, 50, 55, BLACK); // Hapus area kanan

    // ==================================================
    // SPEED NUMBER RENDERING (ANTI-FLICKER)
    // ==================================================
    // Menggunakan trik menulis teks sekalian menimpa warna background hitamnya
    gfx->setTextColor(ICE, BLACK);
    gfx->setTextSize(7); 

    if (speedValue < 10) {
        gfx->setCursor(110, 50);
        gfx->print("00");
    } else if (speedValue < 100) {
        gfx->setCursor(110, 50);
        gfx->print("0");
    } else {
        gfx->setCursor(90, 50);
    }
    gfx->print(speedValue);

    // ==================================================
    // LEFT SIGNAL (Belok Kiri)
    // ==================================================
    if (steerPWM < 1400 && blinkState)
    {
        gfx->fillTriangle(25, 75, 55, 55, 55, 95, YELLOW);
    }

    // ==================================================
    // RIGHT SIGNAL (Belok Kanan)
    // ==================================================
    if (steerPWM > 1600 && blinkState)
    {
        gfx->fillTriangle(295, 75, 265, 55, 265, 95, YELLOW);
    }

    delay(15);
}
