#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// ======================================================
// WAVESHARE ESP32-C6 1.47"
// RACING DASHBOARD WITH DYNAMIC NEON SPEED BAR
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
// STATIC UI (Digambar sekali di awal)
// ======================================================
void drawStaticUI()
{
    gfx->fillScreen(BLACK);

    // KM/H TEXT
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
    // REFRESH AREA SEIN
    // ==================================================
    gfx->fillRect(15, 50, 45, 55, BLACK);  
    gfx->fillRect(260, 50, 50, 55, BLACK); 

    // ==================================================
    // SPEED NUMBER RENDERING
    // ==================================================
    gfx->setTextColor(ICE, BLACK);
    gfx->setTextSize(7); 
    gfx->setCursor(105, 50); 

    if (speedValue < 10) {
        gfx->print("00");
    } else if (speedValue < 100) {
        gfx->print("0");
    }
    gfx->print(speedValue);

    // ==================================================
    // LOGIKA NEON BERJALAN DINAMIS (DYNAMIC BAR)
    // Panjang total bar = 120 piksel (Mulai dari X=100 sampai X=220)
    // ==================================================
    // Mengonversi nilai speedValue (0-120) menjadi panjang pixel (0-120 pixel)
    int barWidth = map(speedValue, 0, 120, 0, 120); 

    // Gambar segmen warna neon yang aktif sesuai panjang gas
    for (int i = 0; i < barWidth; i++) 
    {
        uint16_t segmentColor;
        if (i < 40) {
            segmentColor = GREEN;   // 40 piksel pertama warna hijau
        } else if (i < 80) {
            segmentColor = YELLOW;  // 40 piksel kedua warna kuning
        } else {
            segmentColor = RED;     // Sisanya warna merah
        }
        // Gambar garis vertikal tipis setinggi 4 piksel untuk membentuk bar
        gfx->drawFastVLine(100 + i, 112, 4, segmentColor);
    }

    // Hapus sisa bar di sebelah kanan agar saat lepas gas, neonnya ikut mundur memendek
    if (barWidth < 120) {
        gfx->fillRect(100 + barWidth, 112, 120 - barWidth, 4, BLACK);
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
