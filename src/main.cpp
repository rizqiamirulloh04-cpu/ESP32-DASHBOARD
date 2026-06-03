#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <lvgl.h>

//
// ===================== LCD CONFIG =====================
// WAVESHARE ESP32-C6-LCD-1.47
//

#define TFT_BL   22

#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS   14
#define TFT_DC   15
#define TFT_RST  21

#define SCR_W 320
#define SCR_H 172

//
// ===================== RGB COLORS =====================
//

#define CYAN      lv_color_hex(0x00D5FF)
#define BLUE2     lv_color_hex(0x0088FF)
#define DARK_BG   lv_color_hex(0x02060A)
#define GREEN2    lv_color_hex(0x00FF66)
#define RED2      lv_color_hex(0xFF2020)
#define WHITE2    lv_color_hex(0xFFFFFF)

//
// ===================== DISPLAY =====================
//

Arduino_DataBus *bus = new Arduino_ESP32SPI(
    TFT_DC,
    TFT_CS,
    TFT_SCLK,
    TFT_MOSI,
    GFX_NOT_DEFINED
);

Arduino_GFX *gfx = new Arduino_ST7789(
    bus,
    TFT_RST,
    0,
    true,
    SCR_W,
    SCR_H
);

//
// ===================== LVGL =====================
//

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[SCR_W * 40];

lv_obj_t *speedLabel;
lv_obj_t *rpmBar;
lv_obj_t *batteryLabel;
lv_obj_t *tempLabel;
lv_obj_t *arc;

int speedValue = 0;
int rpmValue = 0;
int batteryValue = 89;
int tempValue = 36;

//
// ===================== FLUSH =====================
//

void my_flush_cb(lv_disp_drv_t *disp,
                 const lv_area_t *area,
                 lv_color_t *color_p)
{
    uint32_t w = area->x2 - area->x1 + 1;
    uint32_t h = area->y2 - area->y1 + 1;

    gfx->draw16bitRGBBitmap(
        area->x1,
        area->y1,
        (uint16_t *)&color_p->full,
        w,
        h
    );

    lv_disp_flush_ready(disp);
}

//
// ===================== UI =====================
//

void createUI()
{
    lv_obj_set_style_bg_color(lv_scr_act(), DARK_BG, 0);
    lv_obj_set_style_bg_opa(lv_scr_act(), LV_OPA_COVER, 0);

    //
    // TITLE
    //

    lv_obj_t *title = lv_label_create(lv_scr_act());
lv_label_set_text(title, "SPORT MODE");
lv_obj_set_style_text_color(title, CYAN, 0);
lv_obj_set_style_text_font(title, LV_FONT_DEFAULT, 0);
lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 8);

    //
    // ARC SPEEDOMETER
    //

    arc = lv_arc_create(lv_scr_act());

    lv_obj_set_size(arc, 220, 220);
    lv_obj_center(arc);

    lv_arc_set_rotation(arc, 135);
    lv_arc_set_bg_angles(arc, 0, 270);

    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);

    lv_obj_set_style_arc_width(arc, 10, LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc, 12, LV_PART_INDICATOR);

    lv_obj_set_style_arc_color(
        arc,
        lv_color_hex(0x222222),
        LV_PART_MAIN
    );

    lv_obj_set_style_arc_color(
        arc,
        CYAN,
        LV_PART_INDICATOR
    );

    lv_arc_set_range(arc, 0, 120);

    //
    // SPEED LABEL
    //

    speedLabel = lv_label_create(lv_scr_act());

lv_label_set_text(speedLabel, "000");

lv_obj_set_style_text_font(
    speedLabel,
    LV_FONT_DEFAULT,
    0
);

lv_obj_set_style_text_color(
    speedLabel,
    WHITE2,
    0
);

lv_obj_align(speedLabel, LV_ALIGN_CENTER, 0, -20);

    //
    // KMH
    //

    lv_obj_t *kmh = lv_label_create(lv_scr_act());

lv_label_set_text(kmh, "KM/H");

lv_obj_set_style_text_color(kmh, WHITE2, 0);

lv_obj_set_style_text_font(
    kmh,
    LV_FONT_DEFAULT,
    0
);

lv_obj_align(kmh, LV_ALIGN_CENTER, 0, 40);

    //
    // RPM BAR
    //

    rpmBar = lv_bar_create(lv_scr_act());

    lv_obj_set_size(rpmBar, 120, 14);

    lv_obj_align(rpmBar, LV_ALIGN_BOTTOM_MID, 0, -22);

    lv_bar_set_range(rpmBar, 0, 100);

    lv_obj_set_style_bg_color(
        rpmBar,
        lv_color_hex(0x222222),
        LV_PART_MAIN
    );

    lv_obj_set_style_bg_color(
        rpmBar,
        GREEN2,
        LV_PART_INDICATOR
    );

    //
    // BATTERY PANEL
    //

    lv_obj_t *batPanel = lv_obj_create(lv_scr_act());

    lv_obj_set_size(batPanel, 70, 48);

    lv_obj_align(batPanel, LV_ALIGN_TOP_RIGHT, -8, 36);

    lv_obj_set_style_bg_color(
        batPanel,
        lv_color_hex(0x081018),
        0
    );

    lv_obj_set_style_border_color(
        batPanel,
        CYAN,
        0
    );

    batteryLabel = lv_label_create(batPanel);

    lv_label_set_text(batteryLabel, "89%");

    lv_obj_set_style_text_color(
        batteryLabel,
        GREEN2,
        0
    );

    lv_obj_center(batteryLabel);

    //
    // TEMP PANEL
    //

    lv_obj_t *tempPanel = lv_obj_create(lv_scr_act());

    lv_obj_set_size(tempPanel, 70, 48);

    lv_obj_align(tempPanel, LV_ALIGN_BOTTOM_RIGHT, -8, -8);

    lv_obj_set_style_bg_color(
        tempPanel,
        lv_color_hex(0x081018),
        0
    );

    lv_obj_set_style_border_color(
        tempPanel,
        CYAN,
        0
    );

    tempLabel = lv_label_create(tempPanel);

    lv_label_set_text(tempLabel, "36C");

    lv_obj_set_style_text_color(
        tempLabel,
        RED2,
        0
    );

    lv_obj_center(tempLabel);
}

//
// ===================== UPDATE UI =====================
//

void updateUI()
{
    speedValue++;

    if (speedValue > 120)
        speedValue = 0;

    rpmValue = map(speedValue, 0, 120, 0, 100);

    lv_arc_set_value(arc, speedValue);

    char buf[16];

    sprintf(buf, "%03d", speedValue);
    lv_label_set_text(speedLabel, buf);

    lv_bar_set_value(rpmBar, rpmValue, LV_ANIM_ON);

    lv_timer_handler();
}

//
// ===================== SETUP =====================
//

void setup()
{
    Serial.begin(115200);

    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    gfx->begin();
    gfx->setRotation(1);

    gfx->fillScreen(0x0000);

    lv_init();

    lv_disp_draw_buf_init(
        &draw_buf,
        buf1,
        NULL,
        SCR_W * 40
    );

    static lv_disp_drv_t disp_drv;

    lv_disp_drv_init(&disp_drv);

    disp_drv.hor_res = SCR_W;
    disp_drv.ver_res = SCR_H;

    disp_drv.flush_cb = my_flush_cb;

    disp_drv.draw_buf = &draw_buf;

    lv_disp_drv_register(&disp_drv);

    createUI();
}

//
// ===================== LOOP =====================
//

void loop()
{
    updateUI();

    delay(30);
}