#include "lvgl.h"

// --- DATA BACKGROUND ---
const uint8_t bg_speedo_map[] = {
    // ... [PASTE SEMUA DATA HEX KAMU DI SINI] ...
};

const lv_img_dsc_t bg_speedo = {
  .header.always_zero = 0,
  .header.w = 320, // Sesuaikan dengan lebar gambarmu
  .header.h = 172, // Sesuaikan dengan tinggi gambarmu
  .data_size = 320 * 172 * 4,
  .header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
  .data = (const uint8_t *)bg_speedo_map,
};

// --- DATA JARUM (NEEDLE) ---
const uint8_t needle_speed_map[] = {
    // ... [PASTE DATA HEX JARUM DI SINI] ...
};

const lv_img_dsc_t needle_speed = {
  .header.always_zero = 0,
  .header.w = 20, // Contoh lebar jarum
  .header.h = 50, // Contoh tinggi jarum
  .data_size = 20 * 50 * 4,
  .header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
  .data = (const uint8_t *)needle_speed_map,
};
