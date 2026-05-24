#include <Arduino.h>

#include <WiFi.h>

#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

#include <Arduino_GFX_Library.h>

#include <lvgl.h>

#include "CyberUI.h"
#include "WebDashboard.h"
#include "LVGLDashboard.h"

// ======================================================
// HOTSPOT
// ======================================================

const char* ap_ssid = "ESP32-DASHBOARD";
const char* ap_password = "12345678";

// ======================================================
// RECEIVER PINS
// ======================================================

#define STEERING_PIN 2
#define THROTTLE_PIN 3

// ======================================================
// LCD PINS WAVESHARE 1.47
// ======================================================

#define TFT_BL   15

#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_DC   8
#define TFT_RST  9
#define TFT_CS   14

// ======================================================
// DISPLAY
// ======================================================

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
    172,
    320,
    34,
    0,
    34,
    0
);

// ======================================================
// LVGL BUFFER
// ======================================================

static lv_color_t buf1[320 * 20];

// ======================================================
// WEB SERVER
// ======================================================

AsyncWebServer server(80);

// ======================================================
// RECEIVER DATA
// ======================================================

volatile int throttlePWM = 1500;
volatile int steeringPWM = 1500;

int speedKMH = 0;
int steeringAngle = 0;

// ======================================================
// HTML DASHBOARD
// ======================================================

const char index_html[] PROGMEM = R"rawliteral(

<!DOCTYPE html>
<html>

<head>

<meta name="viewport" content="width=device-width, initial-scale=1">

<title>ESP32 Dashboard</title>

<style>

body{
    margin:0;
    padding:0;

    background:#0f0f0f;
    color:white;

    font-family:Arial;

    text-align:center;
}

.card{
    width:320px;

    margin:auto;

    margin-top:40px;

    background:#1b1b1b;

    border-radius:24px;

    padding:24px;
}

h1{
    color:cyan;
}

.footer{
    margin-top:20px;
    opacity:0.5;
    font-size:12px;
}

</style>

</head>

<body>

<div class="card">

<h1>ESP32-DASHBOARD</h1>

<p>
ESP32-C6 Dashboard<br>
Waveshare LCD 1.47
</p>

<div class="footer">
WEB DASHBOARD READY
</div>

</div>

</body>
</html>

)rawliteral";

// ======================================================
// LVGL DISPLAY FLUSH
// ======================================================

void my_disp_flush(
    lv_display_t *disp,
    const lv_area_t *area,
    uint8_t *px_map)
{
    uint32_t width =
        area->x2 - area->x1 + 1;

    uint32_t height =
        area->y2 - area->y1 + 1;

    gfx->draw16bitRGBBitmap(
        area->x1,
        area->y1,
        (uint16_t *)px_map,
        width,
        height
    );

    lv_display_flush_ready(disp);
}

// ======================================================
// READ RECEIVER
// ======================================================

void readReceiver()
{
    throttlePWM =
        pulseIn(
            THROTTLE_PIN,
            HIGH,
            25000
        );

    steeringPWM =
        pulseIn(
            STEERING_PIN,
            HIGH,
            25000
        );

    if(throttlePWM == 0)
        throttlePWM = 1500;

    if(steeringPWM == 0)
        steeringPWM = 1500;

    // SPEED

    speedKMH = map(
        throttlePWM,
        1000,
        2000,
        0,
        180
    );

    speedKMH =
        constrain(
            speedKMH,
            0,
            180
        );

    // STEERING

    steeringAngle = map(
        steeringPWM,
        1000,
        2000,
        -45,
        45
    );

    steeringAngle =
        constrain(
            steeringAngle,
            -45,
            45
        );
}

// ======================================================
// SETUP
// ======================================================

void setup()
{
    Serial.begin(115200);

    // RECEIVER

    pinMode(
        STEERING_PIN,
        INPUT
    );

    pinMode(
        THROTTLE_PIN,
        INPUT
    );

    // BACKLIGHT

    pinMode(
        TFT_BL,
        OUTPUT
    );

    digitalWrite(
        TFT_BL,
        HIGH
    );

    // DISPLAY

    gfx->begin();

    gfx->fillScreen(
        BLACK
    );

    // LVGL

    lv_init();

    lv_display_t *disp =
        lv_display_create(
            320,
            172
        );

    lv_display_set_flush_cb(
        disp,
        my_disp_flush
    );

    lv_display_set_buffers(
        disp,
        buf1,
        NULL,
        sizeof(buf1),
        LV_DISPLAY_RENDER_MODE_PARTIAL
    );

    // UI

    initCyberUI();

    initLVGLDashboard();

    // WIFI AP

    WiFi.softAP(
        ap_ssid,
        ap_password
    );

    IPAddress IP =
        WiFi.softAPIP();

    Serial.println(IP);

    // SHOW IP

    gfx->setTextColor(
        WHITE
    );

    gfx->setTextSize(2);

    gfx->setCursor(
        10,
        150
    );

    gfx->println(IP);

    // WEB SERVER

    server.on(
        "/",
        WebRequestMethod::HTTP_GET,
        [](AsyncWebServerRequest *request)
        {
            request->send(
                200,
                "text/html",
                index_html
            );
        }
    );

    initWebDashboard();

    server.begin();

    Serial.println(
        "SERVER STARTED"
    );
}

// ======================================================
// LOOP
// ======================================================

void loop()
{
    // READ RECEIVER

    readReceiver();

    // UPDATE UI

    updateCyberUI();

    updateLVGLDashboard(
        speedKMH,
        steeringAngle
    );

    // LVGL

    lv_timer_handler();

    // WEB

    handleWebDashboard();

    // DEBUG

    Serial.print(
        "Throttle PWM: "
    );

    Serial.print(
        throttlePWM
    );

    Serial.print(
        " | Steering PWM: "
    );

    Serial.print(
        steeringPWM
    );

    Serial.print(
        " | Speed: "
    );

    Serial.print(
        speedKMH
    );

    Serial.print(
        " km/h"
    );

    Serial.print(
        " | Steering: "
    );

    Serial.println(
        steeringAngle
    );

    delay(20);
}