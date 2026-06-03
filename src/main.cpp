#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// ======================================================================
// WAVESHARE ESP32-C6 1.47" - SINGLE FILE PREMIUM GOTHIC DASHBOARD
// ======================================================================

#define TFT_BL 22
#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS   14
#define TFT_DC   15
#define TFT_RST  21

#define STEER_PIN    1
#define THROTTLE_PIN 2

// COLORS (Format RGB565 16-Bit)
#define BLACK          0x0000
#define WHITE          0xFFFF
#define RED            0xF800
#define GREEN          0x07E0 
#define YELLOW         0xFFE0
#define GLOW_BLUE_3    0x0012 

// Dimensi Piksel Per Angka Gothic (Lebar: 30, Tinggi: 55)
#define IMG_W 30
#define IMG_H 55
#define TOTAL_PIXELS (IMG_W * IMG_H) // 1650 piksel per angka

// ======================================================================
// DATA BINAR GAMBAR ANGKA GOTHIC KUSTOM (RGB565 - 16BIT)
// ======================================================================
// Catatan: Jika kamu sudah punya data hex asli hasil konversi gambarmu, 
// kamu bisa mengganti baris angka di dalam kurung kurawal {...} di bawah ini.

const uint16_t gothic_0[TOTAL_PIXELS] PROGMEM = {
    0x0000, 0x18c3, 0x3186, 0x4208, 0x4208, 0x4208, 0x4a49, 0x4208, 0x4208, 0x39c7,
    // ... data diulang atau diteruskan sampai total 1650 elemen (30x55)
    [0 ... (TOTAL_PIXELS - 1)] = 0x0000 // Inisialisasi sisa piksel dengan warna hitam aman
};

const uint16_t gothic_1[TOTAL_PIXELS] PROGMEM = { 
    0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    [0 ... (TOTAL_PIXELS - 1)] = 0x0000 
};

const uint16_t gothic_2[TOTAL_PIXELS] PROGMEM = { 
    0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    [0 ... (TOTAL_PIXELS - 1)] = 0x0000 
};

const uint16_t gothic_3[TOTAL_PIXELS] PROGMEM = { 
    0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    [0 ... (TOTAL_PIXELS - 1)] = 0x0000 
};

const uint16_t gothic_4[TOTAL_PIXELS] PROGMEM = { 
    0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    [0 ... (TOTAL_PIXELS - 1)] = 0x0000 
};

const uint16_t gothic_5[TOTAL_PIXELS] PROGMEM = { 
    0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    [0 ... (TOTAL_PIXELS - 1)] = 0x0000 
};

const uint16_t gothic_6[TOTAL_PIXELS] PROGMEM = { 
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000,
    [0 ... (TOTAL_PIXELS - 1)] = 0x0000 
};

const uint16_t gothic_7[TOTAL_PIXELS] PROGMEM = { 
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000,
    [0 ... (TOTAL_PIXELS - 1)] = 0x0000 
};

const uint16_t gothic_8[TOTAL_PIXELS] PROGMEM = { 
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000,
    [0 ... (TOTAL_PIXELS - 1)] = 0x0000 
};

const uint16_t gothic_9[TOTAL_PIXELS] PROGMEM = { 
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000,
    [0 ... (TOTAL_PIXELS - 1)] = 0x0000 
};

// Array penampung pointer gothic ke indeks 0-9 
const uint16_t* const gothicNumbers[10] PROGMEM = {
    gothic_0, gothic_1, gothic_2, gothic_3, gothic_4,
    gothic_5, gothic_6, gothic_7, gothic_8, gothic_9
};

// ======================================================================
// INITIALISASI HARDWARE DISPLAY & KANVAS
// ======================================================================

Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, GFX_NOT_DEFINED);
Arduino_GFX *gfx = new Arduino_ST7789(bus, TFT_RST, 1, true, 172, 320, 34, 0, 34, 0);

// Canvas internal anti-flicker (Menghindari layar berkedip saat angka berganti cepat)
#define CANVAS_W 140
#define CANVAS_H 60
Arduino_Canvas *canvas = new Arduino_Canvas(CANVAS_W, CANVAS_H, gfx);

int speedValue = 0; 
int lastSpeedValue = -1; 
int targetSpeed = 0;
bool blinkState = false;
unsigned long blinkTimer = 0;
int lastSteerState = 0; 

// Fungsi menggambar pendaran lingkaran biru khas dasbor cyberpunk
void drawCyberpunkGlow() {
    for (int r = 85; r > 0; r -= 3) {
        uint16_t glowColor = BLACK;
        if (r > 65)       glowColor = 0x0004;
        else if (r > 45)  glowColor = 0x000A;
        else if (r > 25)  glowColor = 0x0012;
        else              glowColor = 0x011A;
        gfx->fillCircle(160, 80, r, glowColor);
    }
}

void setup() {
    Serial.begin(115200);
    
    // Mengatur backlight layar ST7789 menggunakan fitur LEDC ESP32-C6
    ledcAttach(TFT_BL, 5000, 8);
    ledcWrite(TFT_BL, 128); 

    gfx->begin();
    gfx->invertDisplay(false);
    gfx->setRotation(1); // Set ke mode Landscape

    canvas->begin();
    gfx->fillScreen(BLACK);
    drawCyberpunkGlow();

    // Teks KM/H Statis di bawah lingkaran angka
    gfx->setTextColor(WHITE); 
    gfx->setTextSize(2);
    gfx->setCursor(136, 125); 
    gfx->print("KM/H");

    pinMode(STEER_PIN, INPUT);
    pinMode(THROTTLE_PIN, INPUT);
}

void loop() {
    // Membaca sinyal PWM dari Receiver remot RC
    int steerPWM = pulseIn(STEER_PIN, HIGH, 20000);
    int throttlePWM = pulseIn(THROTTLE_PIN, HIGH, 20000);

    // Proteksi failsafe: Jika tidak ada sinyal, asumsikan posisi netral (1500us)
    if (steerPWM == 0)     steerPWM = 1500;
    if (throttlePWM == 0)  throttlePWM = 1500; 

    // Filter logika konversi gas remot ke Kecepatan Target
    if (throttlePWM < 1490) { // Maju
        targetSpeed = map(throttlePWM, 1500, 1000, 0, 120);
        targetSpeed = constrain(targetSpeed, 0, 120);
    } else if (throttlePWM > 1510) { // Mundur
        targetSpeed = map(throttlePWM, 1500, 2000, 0, 50);
        targetSpeed = constrain(targetSpeed, 0, 50);
    } else {
        targetSpeed = 0; // Netral
    }

    // Efek pergerakan transisi angka dibuat berangsur/mulus (Smoothing effect)
    if (speedValue < targetSpeed) {
        speedValue += 4; 
        if (speedValue > targetSpeed) speedValue = targetSpeed;
    }
    if (speedValue > targetSpeed) {
        speedValue -= 4;
        if (speedValue < targetSpeed) speedValue = targetSpeed;
    }

    // Mengatur interval waktu kedipan sein (350 milidetik)
    bool toggleBlink = false;
    if (millis() - blinkTimer > 350) {
        blinkTimer = millis();
        blinkState = !blinkState;
        toggleBlink = true; 
    }

    // Deteksi arah kemudi: 0 = Lurus, 1 = Belok Kiri, 2 = Belok Kanan
    int currentSteerState = 0; 
    if (steerPWM < 1400)      currentSteerState = 1;
    else if (steerPWM > 1600) currentSteerState = 2;

    // Manajemen penghapusan area sein agar tidak berkedip redup akibat loop konstan
    if (currentSteerState != lastSteerState || toggleBlink) {
        if (!blinkState || currentSteerState == 0) {
            gfx->fillRect(15, 50, 45, 55, BLACK);  
            gfx->fillRect(260, 50, 50, 55, BLACK); 
        }
        lastSteerState = currentSteerState;
    }

    // ==================================================================
    // PROSES MERENDAER TRANSPARENT GOTHIC DIGIT KE KANVAS
    // ==================================================================
    if (speedValue != lastSpeedValue) {
        
        // 1. Bersihkan area Kanvas internal
        canvas->fillScreen(BLACK);
        
        // 2. Gambar ulang efek pendaran lingkaran di Kanvas agar menyatu alami dengan background
        for (int r = 85; r > 0; r -= 4) {
            uint16_t glowColor = BLACK;
            if (r > 65)       glowColor = 0x0004;
            else if (r > 45)  glowColor = 0x000A;
            else if (r > 25)  glowColor = 0x0012;
            else              glowColor = 0x011A;
            canvas->fillCircle(70, 30, r, glowColor);
        }

        // 3. Pecah angka bulat (0-120) menjadi 3 bagian digit terpisah
        int digit1 = speedValue / 100;          // Ratusan
        int digit2 = (speedValue / 10) % 10;    // Puluhan
        int digit3 = speedValue % 10;           // Satuan

        // 4. Tempel potongan gambar biner angka secara berdampingan di kanvas
        canvas->draw16bitRGBBitmap(20, 3, (uint16_t*)pgm_read_ptr(&gothicNumbers[digit1]), IMG_W, IMG_H);
        canvas->draw16bitRGBBitmap(55, 3, (uint16_t*)pgm_read_ptr(&gothicNumbers[digit2]), IMG_W, IMG_H);
        canvas->draw16bitRGBBitmap(90, 3, (uint16_t*)pgm_read_ptr(&gothicNumbers[digit3]), IMG_W, IMG_H);

        // 5. Kirim gambar hasil gabungan kanvas utuh ke tengah layar LCD utama
        gfx->draw16bitRGBBitmap(90, 45, canvas->getFramebuffer(), CANVAS_W, CANVAS_H);

        lastSpeedValue = speedValue;
    }

    // Indikator Neon Bar Gradasi Warna (Hijau -> Kuning -> Merah)
    int barWidth = map(speedValue, 0, 120, 0, 120); 
    for (int i = 0; i < barWidth; i++) {
        uint16_t segmentColor = GREEN;
        if (i >= 50 && i < 90) segmentColor = YELLOW;
        else if (i >= 90)      segmentColor = RED;
        gfx->drawFastVLine(100 + i, 114, 4, segmentColor);
    }
    // Sisa bar yang tidak aktif diberi warna redup (pendaran dasar)
    if (barWidth < 120) {
        gfx->fillRect(100 + barWidth, 114, 120 - barWidth, 4, GLOW_BLUE_3);
    }

    // Gambar Segitiga Lampu Sein Kuning (Menyala stabil saat siklus blinkState aktif)
    if (currentSteerState == 1 && blinkState) {
        gfx->fillTriangle(25, 75, 55, 55, 55, 95, YELLOW);
    }
    if (currentSteerState == 2 && blinkState) {
        gfx->fillTriangle(295, 75, 265, 55, 265, 95, YELLOW);
    }

    delay(5); 
}
