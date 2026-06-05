#include "ui_classic.h"

// Pointer untuk objek UI
lv_obj_t * screen_classic;
lv_obj_t * img_bg;
lv_obj_t * needle_s, * needle_r, * needle_b, * needle_t;

void create_screen_classic() {
    // 1. Buat Screen
    screen_classic = lv_obj_create(NULL);
    
    // 2. Tampilkan Background (Cluster 3 Lingkaran)
    img_bg = lv_img_create(screen_classic);
    lv_img_set_src(img_bg, &bg_speedo); // Pastikan nama variabel ini sesuai di assets.c
    lv_obj_center(img_bg);

    // 3. Tampilkan Jarum (Speedometer)
    needle_s = lv_img_create(screen_classic);
    lv_img_set_src(needle_s, &needle_speed); // Gambar jarum speedometer
    lv_obj_set_pos(needle_s, 50, 50);        // Sesuaikan koordinat x,y agar pas di tengah lingkaran
    lv_img_set_pivot(needle_s, 10, 50);      // Titik poros jarum (x, y)

    // (Lakukan hal yang sama untuk needle_r, needle_b, needle_t di posisi masing-masing)
}

void update_gauges(int speed, int rpm, int batt, int temp) {
    // 4. Update Jarum Speedometer
    // map(nilai, min_data, max_data, min_sudut, max_sudut)
    // 0-240 km/h -> 0-270 derajat (gunakan skala 10 untuk LVGL: 0-2700)
    int angle_s = map(speed, 0, 240, 0, 2700);
    lv_img_set_angle(needle_s, angle_s);

    // (Tambahkan logika map yang sama untuk rpm, batt, temp di sini)
}
