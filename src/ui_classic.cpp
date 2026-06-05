#include "ui_classic.h"

lv_obj_t * screen_classic;
lv_obj_t * needle_s;

void create_screen_classic() {
    screen_classic = lv_obj_create(NULL);
    
    // 1. Membuat Lingkaran Gauge (Background)
    lv_obj_t * arc = lv_arc_create(screen_classic);
    lv_obj_set_size(arc, 150, 150);
    lv_arc_set_bg_angles(arc, 135, 45);
    lv_obj_center(arc);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB); // Hapus tombol lingkaran
    
    // 2. Membuat Jarum (menggunakan objek line)
    needle_s = lv_obj_create(screen_classic);
    lv_obj_set_size(needle_s, 2, 70); // Tebal 2px, Panjang 70px
    lv_obj_set_style_bg_color(needle_s, lv_color_hex(0xFF0000), 0); // Warna Merah
    lv_obj_align(needle_s, LV_ALIGN_CENTER, 0, -35); // Posisikan di tengah
    
    // Kita gunakan transformasi untuk rotasi
    lv_obj_set_pivot(needle_s, 1, 69); // Poros di ujung bawah jarum
}

void update_gauges(int speed) {
    // Rotasi jarum (sesuaikan nilai map dengan sensor kamu)
    int angle = map(speed, 0, 240, 135, 405); 
    lv_obj_set_style_transform_angle(needle_s, angle * 10, 0);
}
