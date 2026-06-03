#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// ======================================================
// WAVESHARE ESP32-C6 1.47"
// FLICKER-FREE RACING DASHBOARD (USING CANVAS)
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
// DISPLAY & CANVAS CONFIGURATION
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

// Membuat Canvas sebesar resolusi layar (320x172) di memori internal ESP32
Arduino_Canvas *canvas = new Arduino_Canvas(320, 172, gfx);

// ======================================================
// VARIABLES
// ======================================================
int speedValue = 0;
int targetSpeed = 0;

bool blinkState = false;
unsigned long blinkTimer = 0;

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
    // DISPLAY & CANVAS INITIALIZATION
    // ==================================================
    gfx->begin();
    gfx->invertDisplay(false);
    gfx->setRotation(1); 

    canvas->begin(); // Mengaktifkan canvas memory
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
    // DUAL MAPPING (MAJU 0-120, MUNDUR 0-50)
    // ==================================================
    if (throttlePWM < 1480) 
    {
        targetSpeed = map(throttlePWM, 1500, 1000, 0, 120);
        targetSpeed = constrain(targetSpeed, 0, 120);
    } 
    else if (throttlePWM > 1520) 
    {
        targetSpeed = map(throttlePWM, 1500, 2000, 0, 50);
        targetSpeed = constrain(targetSpeed, 0, 50);
    } 
    else 
    {
        targetSpeed = 0;
    }

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
    // PROSES MENGGAMBAR DI CANVAS (BELUM TAMPIL DI LAYAR)
    // Semua perintah di bawah menggunakan 'canvas->' bukan 'gfx->'
    // ==================================================
    
    // 1. Bersihkan seluruh canvas background jadi hitam bersih
    canvas->fillScreen(BLACK);

    // 2. SPEED NUMBER
    canvas->setTextColor(ICE);
    canvas->setTextSize(7); 

    if (speedValue < 10) {
        canvas->setCursor(110, 50);
        canvas->print("00");
    } else if (speedValue < 100) {
        canvas->setCursor(110, 50);
        canvas->print("0");
    } else {
        canvas->setCursor(90, 50);
    }
    canvas->print(speedValue);

    // 3. KM/H TEXT
    canvas->setTextColor(CYAN);
    canvas->setTextSize(2);
    canvas->setCursor(136, 120); 
    canvas->print("KM/H");

    // 4. GLOW LINE
    canvas->drawFastHLine(110, 110, 100, DARK);

    // 5. LEFT SIGNAL
    if (steerPWM < 1400 && blinkState)
    {
        canvas->fillTriangle(25, 75, 55, 55, 55, 95, YELLOW);
    }

    // 6. RIGHT SIGNAL
    if (steerPWM > 1600 && blinkState)
    {
        canvas->fillTriangle(295, 75, 265, 55, 265, 95, YELLOW);
    }

    // ==================================================
    // KIRIM HASIL AKHIR KE LAYAR UTAMA (FLUSH)
    // ==================================================
    // Perintah ini menyalin seluruh isi canvas ke fisik layar sekaligus
    canvas->flush();

    delay(10); // Jeda diperkecil sedikit agar animasi fps terasa makin fluid
}
