#ifndef UI_CLASSIC_H
#define UI_CLASSIC_H

#include <Arduino_GFX_Library.h>

// Deklarasi extern agar file .cpp lain tahu variabel ini ada di main.cpp
extern Arduino_DataBus *bus;
extern Arduino_GFX *gfx;
extern Arduino_Canvas *canvas;

// Prototipe fungsi
void draw_ui_classic(int speed, int rpm, int bat, int sig, int temp, bool blink);

#endif
