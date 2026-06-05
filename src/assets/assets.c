#include "lvgl.h"

// Data array untuk background (hasil konversi png)
const lv_img_dsc_t bg_speedo = {
  .header.always_zero = 0,
  .header.w = 320, // Lebar gambar
  .header.h = 172, // Tinggi gambar
  .data_size = 320 * 172 * 4,
  .header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
  .data = (const uint8_t *)bg_speedo_map,
};

// Data array piksel (ini yang panjang sekali)
const uint8_t bg_speedo_map[] = {
  0x00, 0x00, 0xFF, 0x00, // Contoh data piksel
  0x1A, 0x2B, 0x3C, 0xFF,
  // ... ribuan baris lainnya ...
};

// Lakukan hal yang sama untuk needle_speed, needle_rpm, dll.
