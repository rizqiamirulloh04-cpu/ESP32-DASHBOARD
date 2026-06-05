#include "ui_classic.h"
#include <Arduino.h> // WAJIB untuk fungsi map()

lv_obj_t * needle_s;

void create_screen_classic() {
    lv_obj_t * screen = lv_scr_act();
    
    // ... kode objek lain ...

    // Ganti lv_obj_set_pivot menjadi lv_img_set_pivot 
    // Pastikan needle_s adalah lv_img_create
    needle_s = lv_img_create(screen);
    lv_img_set_src(needle_s, &needle_speed); 
    lv_img_set_pivot(needle_s, 10, 60); // Sesuaikan titik poros (x, y)
    lv_obj_align(needle_s, LV_ALIGN_CENTER, 0, 0);
}

void update_gauges(int speed) {
    // Membatasi nilai agar jarum tidak berputar berlebihan
    int angle = map(speed, 0, 240, 0, 2700); // 2700 = 270 derajat
    lv_img_set_angle(needle_s, angle);
}
