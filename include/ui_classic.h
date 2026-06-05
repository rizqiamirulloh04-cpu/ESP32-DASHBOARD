#ifndef UI_CLASSIC_H
#define UI_CLASSIC_H

#include <lvgl.h>

// Deklarasi extern agar bisa dipanggil di file lain
extern const lv_img_dsc_t bg_speedo;
extern const lv_img_dsc_t needle_speed;

void create_screen_classic();
void update_gauges(int speed);

#endif
