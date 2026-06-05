#ifndef UI_CLASSIC_H
#define UI_CLASSIC_H

#include <Arduino_GFX_Library.h>

void init_ui(Arduino_GFX *gfx_ptr);
// Pastikan parameter di sini SAMA dengan di file .cpp
void draw_ui_classic(int speed, int rpm, int batt, int sig, int steerState, bool blinkState);

#endif
