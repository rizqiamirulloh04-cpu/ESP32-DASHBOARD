#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// ======================================================
// WAVESHARE ESP32-C6 1.47"
// FINAL RACING DASHBOARD (FULL SCREEN 320x172)
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

// Menggunakan spesifikasi Waveshare ST7789 1.47" (172x320)
Arduino_GFX *gfx = new Arduino_ST7789(
    bus,
    TFT_RST,
    1,     // Rotasi diubah ke 1 agar menjadi Landscape (320x172)
    true,  // IPS = true
    172,   // Lebar fisik asli
    320,   // Tinggi fisik asli
    34,    // Col offset (sesuai bawaan waveshare)
    0,     // Row offset
    34,    // Col offset alternatif
    0      // Row offset alternatif
);

// ======================================================
// VARIABLES
// ======================================================
int speedValue = 0;
int targetSpeed = 0;

bool blinkState = false;
unsigned long blinkTimer = 0;

// ======================================================
// STATIC UI (RE-CENTERED FOR 320x172)
// ======================================================
void drawStaticUI()
{
    gfx->fillScreen(BLACK);

    // ==================================================
    // TOP LINE (Garis tengah atas lebar)
    // ==================================================
    gfx->drawFastHLine(60, 35, 200, CYAN);

    // ==================================================
    // TRIANGLE (Dipindah ke tengah-tengah X=160)
    // ==================================================
    gfx->fillTriangle(
        160, 15,  // Titik atas (Tengah)
        145, 34,  // Titik kiri bawah
        175, 34,  // Titik kanan bawah
        GREEN
    );

    // ==================================================
    // LEFT BAR (Batang merah dipindah ke ujung kiri layar)
    // ==================================================
    gfx->fillRect(20, 35, 6, 115, RED);

    // ==================================================
    // RIGHT BAR (Batang biru dipindah ke ujung kanan layar)
    // ==================================================
    gfx->fillRect(294, 35, 6, 115, BLUE);
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
    ledcWrite(TFT_BL, 18); // Kecerahan backlight

    // ==================================================
    // DISPLAY INITIALIZATION
    // ==================================================
    gfx->begin();
    gfx->invertDisplay(false);
    
    // Memastikan orientasi landscape lebar 320, tinggi 172
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
    // CLEAR CENTER (Hanya menghapus area angka dan teks tengah)
    // ==================================================
    gfx->fillRect(70, 50, 180, 100, BLACK);

    // ==================================================
    // SPEED NUMBER (Disesuaikan agar pas di tengah X=160)
    // ==================================================
    gfx->setTextColor(ICE);
    gfx->setTextSize(7); // Ukuran teks diperbesar ke 7 agar proporsional di layar besar

    // Geser posisi X cursor berdasarkan jumlah digit agar tetap presisi di tengah
    if (speedValue < 10) {
        gfx->setCursor(110, 55);
        gfx->print("00");
    } else if (speedValue < 100) {
        gfx->setCursor(110, 55);
        gfx->print("0");
    } else {
        gfx->setCursor(90, 55);
    }
    gfx->print(speedValue);

    // ==================================================
    // KM/H TEXT
    // ==================================================
    gfx->setTextColor(CYAN);
    gfx->setTextSize(2);
    gfx->setCursor(136, 125); // Diposisikan di tengah bawah angka speed
    gfx->print("KM/H");

    // ==================================================
    // GLOW LINE (Garis horizontal di atas KM/H)
    // ==================================================
    gfx->drawFastHLine(110, 115, 100, DARK);

    // ==================================================
    // LEFT SIGNAL (Diposisikan di sebelah kiri angka speed)
    // ==================================================
    if (steerPWM < 1400 && blinkState)
    {
        gfx->fillTriangle(
            45, 85,   // Ujung panah kiri
            65, 70,   // Kanan atas
            65, 100,  // Kanan bawah
            YELLOW
        );
    }

    // ==================================================
    // RIGHT SIGNAL (Diposisikan di sebelah kanan angka speed)
    // ==================================================
    if (steerPWM > 1600 && blinkState)
    {
        gfx->fillTriangle(
            275, 85,  // Ujung panah kanan
            255, 70,  // Kiri atas
            255, 100, // Kiri bawah
            YELLOW
        );
    }

    delay(15);
}
