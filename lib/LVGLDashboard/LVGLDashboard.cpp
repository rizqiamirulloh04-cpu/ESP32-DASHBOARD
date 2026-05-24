#include <Arduino.h>

#include <lvgl.h>

static lv_obj_t *speedLabel;

static lv_obj_t *steeringLabel;

// ======================================================
// INIT
// ======================================================

void initLVGLDashboard()
{
    lv_obj_set_style_bg_color(
        lv_screen_active(),
        lv_color_hex(0x000000),
        0
    );

    // =========================
    // TITLE
    // =========================

    lv_obj_t *title =
        lv_label_create(
            lv_screen_active()
        );

    lv_label_set_text(
        title,
        "CYBER DASHBOARD"
    );

    lv_obj_set_style_text_color(
        title,
        lv_color_hex(0x00FFFF),
        0
    );

    lv_obj_align(
        title,
        LV_ALIGN_TOP_MID,
        0,
        10
    );

    // =========================
    // SPEED LABEL
    // =========================

    speedLabel =
        lv_label_create(
            lv_screen_active()
        );

    lv_label_set_text(
        speedLabel,
        "0 KM/H"
    );

    lv_obj_set_style_text_color(
        speedLabel,
        lv_color_hex(0xFFFFFF),
        0
    );

    lv_obj_set_style_text_font(
        speedLabel,
        &lv_font_montserrat_28,
        0
    );

    lv_obj_align(
        speedLabel,
        LV_ALIGN_CENTER,
        0,
        -20
    );

    // =========================
    // STEERING LABEL
    // =========================

    steeringLabel =
        lv_label_create(
            lv_screen_active()
        );

    lv_label_set_text(
        steeringLabel,
        "STEER: 0"
    );

    lv_obj_set_style_text_color(
        steeringLabel,
        lv_color_hex(0x00FF00),
        0
    );

    lv_obj_align(
        steeringLabel,
        LV_ALIGN_CENTER,
        0,
        40
    );
}

// ======================================================
// UPDATE
// ======================================================

void updateLVGLDashboard(
    int speed,
    int steering
)
{
    static int lastSpeed = -1;

    static int lastSteering = -999;

    // =========================
    // SPEED UPDATE
    // =========================

    if(speed != lastSpeed)
    {
        lastSpeed = speed;

        String txt =
            String(speed) +
            " KM/H";

        lv_label_set_text(
            speedLabel,
            txt.c_str()
        );
    }

    // =========================
    // STEERING UPDATE
    // =========================

    if(steering != lastSteering)
    {
        lastSteering =
            steering;

        String txt =
            "STEER: " +
            String(steering);

        lv_label_set_text(
            steeringLabel,
            txt.c_str()
        );
    }
}