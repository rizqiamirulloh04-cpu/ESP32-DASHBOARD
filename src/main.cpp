#include <Arduino.h>
#include <Arduino_GFX_Library.h>

#define TFT_BL   22

#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS   14
#define TFT_DC   15
#define TFT_RST  21

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
    3,
    true,
    172,
    320,
    34,
    0,
    34,
    0
);

void setup()
{
    // Backlight redup
    ledcAttach(TFT_BL, 5000, 8);
    ledcWrite(TFT_BL, 40);

    // Init display
    gfx->begin();

    gfx->invertDisplay(false);

    // Background hitam
    gfx->fillScreen(0x0000);

    // Text hijau
    gfx->setTextColor(0x07E0);

    // Ukuran text
    gfx->setTextSize(2);

    // Judul
    gfx->setCursor(5, 5);
    gfx->println("FPS TEST");

    gfx->setCursor(35, 25);
    gfx->println("ESP32-C6");

    gfx->setCursor(25, 50);
    gfx->println("LANDSCAPE");

    // RGB Bar
    gfx->fillRect(15, 120, 40, 15, 0xF800);
    gfx->fillRect(65, 120, 40, 15, 0x07E0);
    gfx->fillRect(115, 120, 40, 15, 0x001F);
}

void loop()
{
    static int x = 20;
    static int old_x = 20;
    static int dir = 1;

    // Hapus bola lama
    gfx->fillCircle(old_x, 85, 18, 0x0000);

    // Gambar bola baru
    gfx->fillCircle(x, 85, 18, 0xF800);

    // Simpan posisi lama
    old_x = x;

    // Gerakkan bola
    x += dir * 4;

    // Pantul kanan
    if (x >= 150)
    {
        dir = -1;
    }

    // Pantul kiri
    if (x <= 20)
    {
        dir = 1;
    }

    delay(10);
}