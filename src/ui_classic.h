#ifndef UI_CLASSIC_H
#define UI_CLASSIC_H

#include <Arduino_GFX_Library.h>

// Gunakan extern: artinya variabel ini dibuat di file lain (main.cpp)
extern Arduino_DataBus *bus;
extern Arduino_GFX *gfx;
extern Arduino_Canvas *canvas;

// Deklarasi fungsi agar bisa dipanggil di main.cpp
void init_ui();
void draw_ui_classic(int speed, int rpm, int bat, int sig, int temp, bool blink);

#endif
