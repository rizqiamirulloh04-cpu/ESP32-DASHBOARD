#ifndef UI_CLASSIC_H
#define UI_CLASSIC_H

#include <lvgl.h>

// Deklarasi gambar (nanti kamu masukkan kode C Array dari converter di sini)
LV_IMG_DECLARE(bg_speedo); 
LV_IMG_DECLARE(needle);

// Fungsi Utama
void create_screen_classic();
void update_gauges(int speed, int rpm, int batt, int temp);

#endif
