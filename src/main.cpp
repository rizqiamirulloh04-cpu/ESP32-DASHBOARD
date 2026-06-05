#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// Definisi pin sesuai hardware asli kamu
#define TFT_BL 22
#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS   14
#define TFT_DC   15
#define TFT_RST  21

Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, GFX_NOT_DEFINED);
Arduino_GFX *gfx = new Arduino_ST7789(bus, TFT_RST, 1, true, 172, 320, 34, 0, 34, 0);

void setup() {
    Serial.begin(115200);
    
    // Paksa backlight nyala
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH); 

    if (!gfx->begin()) {
        Serial.println("gfx->begin() gagal!");
    } else {
        Serial.println("gfx->begin() sukses!");
        gfx->fillScreen(0xF800); // Merah (0xF800 adalah warna merah di RGB565)
        Serial.println("Seharusnya layar sekarang berwarna merah.");
    }
}

void loop() {
    // Kosongkan
}
