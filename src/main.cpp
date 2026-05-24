#include <Arduino.h>

#include <WiFi.h>

#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

#include <Arduino_GFX_Library.h>

#define BLACK     0x0000
#define WHITE     0xFFFF
#define CYAN      0x07FF
#define GREEN     0x07E0
#define DARKGREY  0x7BEF

#include "CyberUI.h"
#include "WebDashboard.h"

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

#define TFT_BL    15

#define TFT_MOSI  6
#define TFT_SCLK  7
#define TFT_DC    8
#define TFT_CS    14

#define TFT_RST   GFX_NOT_DEFINED

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
    0,          // rotation
    true,       // IPS
    172,        // width
    320,        // height
    34,         // col offset 1
    0,          // row offset 1
    34,         // col offset 2
    0           // row offset 2
);

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

pinMode(TFT_BL, OUTPUT);
digitalWrite(TFT_BL, HIGH);

gfx->fillScreen(BLACK);

gfx->setRotation(1);

gfx->setTextColor(WHITE);
gfx->setTextSize(2);

gfx->setCursor(20, 40);
gfx->println("DISPLAY OK");

gfx->setCursor(20, 80);
gfx->println("ESP32-C6");

    gfx->drawRect(
        0,
        0,
        320,
        172,
        CYAN
    );

    // WIFI AP

    WiFi.softAP(
        ap_ssid,
        ap_password
    );

    IPAddress IP =
        WiFi.softAPIP();

    Serial.println(IP);

    // SHOW IP

    gfx->setCursor(
        20,
        90
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