#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// PIN DEFINITION
#define TFT_BL 22
#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS   14
#define TFT_DC   15
#define TFT_RST  21
#define RX_PIN   1  // Pin receiver Anda

Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, GFX_NOT_DEFINED);
Arduino_GFX *gfx = new Arduino_ST7789(bus, TFT_RST, 3, true, 172, 320, 34, 0, 34, 0);
Arduino_Canvas *canvas = new Arduino_Canvas(320, 172, gfx);

void setup() {
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
    pinMode(RX_PIN, INPUT_PULLUP); // Pin input receiver

    gfx->begin();
    canvas->begin();
}

void loop() {
    // 1. Baca Receiver
    int pulse = pulseIn(RX_PIN, HIGH, 20000); 
    int speed = map(pulse, 1000, 2000, 0, 100); // Sesuaikan range receiver Anda

    // 2. Gambar ke Canvas
    canvas->fillScreen(0x0000);
    canvas->drawCircle(160, 86, 80, 0xFFFF); // Lingkaran putih
    
    canvas->setCursor(120, 70);
    canvas->setTextColor(0xFFFF);
    canvas->setTextSize(4);
    canvas->print(speed);
    
    // 3. Update layar fisik
    gfx->draw16bitRGBBitmap(0, 0, canvas->getFramebuffer(), 320, 172);
    
    delay(50);
}
