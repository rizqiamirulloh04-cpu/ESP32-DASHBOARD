#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// 1. PIN DEFINITION
#define TFT_BL 22
#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS   14
#define TFT_DC   15
#define TFT_RST  21

// 2. DRIVER & CANVAS (Konfigurasi Final 1.47" Waveshare)
Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, GFX_NOT_DEFINED);
Arduino_GFX *gfx = new Arduino_ST7789(
  bus, TFT_RST, 3, true, 172, 320, 34, 0, 34, 0
);
Arduino_Canvas *canvas = new Arduino_Canvas(320, 172, gfx);

// 3. FUNGSI GRAFIS SPEEDOMETER
void drawDashboard(int speed, int rpm) {
    canvas->fillScreen(0x0000); // Background Hitam
    
    // Gambar Frame Luar
    canvas->drawCircle(160, 86, 80, 0x7BEF);
    
    // Tampilkan Angka Speed
    canvas->setCursor(130, 70);
    canvas->setTextColor(0xFFFF);
    canvas->setTextSize(4);
    canvas->print(speed);
    
    // Tampilkan Label
    canvas->setCursor(140, 110);
    canvas->setTextSize(1);
    canvas->print("KM/H");
}

void setup() {
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH); // Backlight ON

    delay(1000); // Jeda stabilisasi
    
    gfx->begin();
    canvas->begin();
    canvas->fillScreen(0x0000);
}

void loop() {
    // Simulasi data speedometer
    int currentSpeed = 85; 
    
    drawDashboard(currentSpeed, 3000);
    
    // Kirim data ke layar fisik
    gfx->draw16bitRGBBitmap(0, 0, canvas->getFramebuffer(), 320, 172);
    
    delay(100);
}
