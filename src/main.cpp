#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// ======================================================
// WAVESHARE ESP32-C6 1.47"
// FINAL RACING DASHBOARD (MODIFIED UI)
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
int speedValue = 0;
int targetSpeed = 0;

bool blinkState = false;
unsigned long blinkTimer = 0;

// ======================================================
// STATIC UI
// ======================================================
void drawStaticUI()
{
    // Hanya membersihkan layar menjadi hitam pekat
    // Garis merah, biru, cyan, dan segitiga hijau telah dihapus
    gfx->fillScreen(BLACK);
}

// ======================================================
// SETUP
// ======================================================
void setup()
{
    Serial.begin(115200);

    // ==================================================
    // BACKLIGHT
    // ==================================================
    ledcAttach(TFT_BL, 5000, 8);
    ledcWrite(TFT_BL, 18); 

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
    if (throttlePWM == 0)  throttlePWM = 1000;

    // SPEED MAP
    targetSpeed = map(throttlePWM, 1000, 2000, 0, 120);
    targetSpeed = constrain(targetSpeed, 0, 120);

    // SMOOTHING
    if (speedValue < targetSpeed) speedValue++;
    if (speedValue > targetSpeed) speedValue--;

    // BLINK TIMER
    if (millis() - blinkTimer > 350)
    {
        blinkTimer = millis();
        blinkState = !blinkState;
    }

    // ==================================================
    // CLEAR CENTER & SIGNALS AREA
    // ==================================================
    // Menghapus area tengah termasuk tempat kedipan lampu sein agar tidak berbayang
    gfx->fillRect(15, 45, 290, 110, BLACK);

    // ==================================================
    // SPEED NUMBER (Tengah-Tengah Layar)
    // ==================================================
    gfx->setTextColor(ICE);
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
    // KM/H TEXT
    // ==================================================
    gfx->setTextColor(CYAN);
    gfx->setTextSize(2);
    gfx->setCursor(136, 120); 
    gfx->print("KM/H");

    // ==================================================
    // GLOW LINE
    // ==================================================
    gfx->drawFastHLine(110, 110, 100, DARK);

    // ==================================================
    // LEFT SIGNAL (Di Tengah Samping Kiri Angka Speed)
    // ==================================================
    if (steerPWM < 1400 && blinkState)
    {
        gfx->fillTriangle(
            25, 75,   // Ujung panah kiri (Sejajar tengah vertikal)
            50, 55,   // Kanan atas
            50, 95,   // Kanan bawah
            YELLOW
        );
    }

    // ==================================================
    // RIGHT SIGNAL (Di Tengah Samping Kanan Angka Speed)
    // ==================================================
    if (steerPWM > 1600 && blinkState)
    {
        gfx->fillTriangle(
            295, 75,  // Ujung panah kanan (Sejajar tengah vertikal)
            270, 55,  // Kiri atas
            270, 95,  // Kiri bawah
            YELLOW
        );
    }

    delay(15);
}
