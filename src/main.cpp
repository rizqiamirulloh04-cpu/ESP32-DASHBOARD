#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <math.h>

// 1. PIN DEFINITION (Pastikan ini sesuai dengan board Anda)
#define TFT_BL 22
#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS   14
#define TFT_DC   15
#define TFT_RST  21

// 2. DRIVER & CANVAS (Konfigurasi Offset 34, 0 WAJIB untuk 1.47")
Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, GFX_NOT_DEFINED);
Arduino_GFX *gfx = new Arduino_ST7789(bus, TFT_RST, 3, true, 172, 320, 34, 0, 34, 0);
Arduino_Canvas *canvas = new Arduino_Canvas(320, 172, gfx);

void drawDashboard(int speed, int rpm) {
    // Bersihkan Canvas (bukan langsung layar fisik)
    canvas->fillScreen(0x0000); 

    // A. Gambar Gauge Speedometer
    canvas->drawCircle(85, 85, 70, 0x5AEB); // Lingkaran Gauge
    
    // B. Jarum Speedometer
    int angle = map(constrain(speed, 0, 200), 0, 200, 140, 400);
    float rad = angle * M_PI / 180.0;
    canvas->drawLine(85, 85, 85 + (int)(cos(rad)*55), 85 + (int)(sin(rad)*55), 0xF800);
    canvas->fillCircle(85, 85, 5, 0xF800);

    // C. Bar RPM (Kanan)
    canvas->drawRect(180, 50, 120, 40, 0x5AEB);
    int barWidth = map(constrain(rpm, 0, 100), 0, 100, 0, 116);
    canvas->fillRect(182, 52, barWidth, 36, 0x07E0);
    
    // D. Angka KMH
    canvas->setCursor(210, 110);
    canvas->setTextSize(3);
    canvas->setTextColor(0x07FF);
    canvas->printf("%03d", speed);

    // E. Kirim isi canvas ke layar fisik DALAM SATU PERINTAH
    gfx->draw16bitRGBBitmap(0, 0, canvas->getFramebuffer(), 320, 172);
}

void setup() {
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
    gfx->begin();
    canvas->begin();
}

void loop() {
    // Simulasi data (Ganti dengan input sensor Anda)
    int speed = 85; 
    int rpm = 50;
    
    drawDashboard(speed, rpm);
    delay(30); // Berikan jeda agar prosesor tidak overheat
}
