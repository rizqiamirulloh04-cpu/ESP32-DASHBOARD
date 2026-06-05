#include "ui_classic.h"

// Gunakan 'extern' agar kodenya tahu gfx didefinisikan di tempat lain
extern Arduino_GFX *gfx; 
Arduino_Canvas *canvas;

void init_ui(Arduino_GFX *gfx_ptr) {
    gfx = gfx_ptr;
    canvas = new Arduino_Canvas(320, 172, gfx);
    canvas->begin();
}
// ... sisa kode draw_ui_classic ...
