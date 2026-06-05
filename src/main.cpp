#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// PIN DEFINITION (Sesuaikan dengan Waveshare ESP32-C6 Anda)
#define TFT_BL 22
#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS   14
#define TFT_DC   15
#define TFT_RST  21

// DRIVER SETUP
Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, GFX_NOT_DEFINED);
Arduino_GFX *gfx = new Arduino_ST7789(bus, TFT_RST, 3, true, 172, 320, 34, 0, 34, 0);
Arduino_Canvas *canvas = new Arduino_Canvas(320, 172, gfx);

void drawUI(int speed, int rpm) {
    canvas->fillScreen(0x0000); // Background Hitam

    // 1. Gambar Speedometer Lingkar (Arc)
    canvas->drawArc(80, 86, 70, 0, 150, 390, 0xFFFF);
    
    // 2. Tampilkan Angka KM/H
    canvas->setCursor(60, 70);
    canvas->setTextSize(3);
    canvas->print(speed);
    
    // 3. Bar RPM (Kanan Bawah)
    canvas->drawRect(200, 120, 100, 20, 0xFFFF); // Kotak bingkai RPM
    int barWidth = map(rpm, 0, 8000, 0, 100);    // Hitung lebar bar
    canvas->fillRect(200, 120, barWidth, 20, 0x07E0); // Isi bar warna hijau

    // 4. Indikator Sein (Segitiga Sederhana)
    canvas->fillTriangle(280, 150, 300, 160, 280, 170, 0xF800);
}

void setup() {
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
    gfx->begin();
    canvas->begin();
}

void loop() {
    // Simulasi Data (Ganti dengan input receiver Anda)
    int speed = 48; 
    int rpm = 4000;
    
    drawUI(speed, rpm);
    
    gfx->draw16bitRGBBitmap(0, 0, canvas->getFramebuffer(), 320, 172);
    delay(50);
}
