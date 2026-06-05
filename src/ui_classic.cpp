#include "ui_classic.h"

void draw_ui_classic(int speed, int rpm, int bat, int sig, int temp, bool blink) {
    // Gunakan 'canvas' dan 'gfx' langsung di sini 
    // karena sudah di-extern di file .h
    canvas->fillScreen(0x0000); 
    canvas->setCursor(0, 0);
    canvas->print("Speed: ");
    canvas->print(speed);
    
    // Selesaikan fungsi gambar Anda di sini
}
