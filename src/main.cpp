#include <Arduino.h>
#include "CyberUI.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

#include <Arduino_GFX_Library.h>

// ======================================================
// HOTSPOT
// ======================================================

const char* ap_ssid = "ESP32-DASHBOARD";
const char* ap_password = "12345678";

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
// WEB SERVER
// ======================================================

AsyncWebServer server(80);

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

.button{

    display:inline-block;

    margin-top:20px;

    background:cyan;

    color:black;

    padding:14px 24px;

    border-radius:12px;

    text-decoration:none;

    font-weight:bold;
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
ESP32-C6 OTA Dashboard<br>
Waveshare LCD 1.47
</p>

<a class="button" href="/update">
UPLOAD FIRMWARE
</a>

<div class="footer">
OTA Web Installer Ready
</div>

</div>

</body>
</html>

)rawliteral";

// ======================================================
// SETUP
// ======================================================

void setup()
{
    Serial.begin(115200);

    // =========================
    // BACKLIGHT
    // =========================

    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    // =========================
    // LCD INIT
    // =========================

    gfx->begin();

    gfx->initCyberUI();

    gfx->setTextColor(WHITE);

    gfx->setTextSize(2);

    gfx->setCursor(20, 40);
    gfx->println("ESP32-DASHBOARD");

    gfx->setCursor(20, 80);
    gfx->println("Starting AP...");

    // =========================
    // WIFI AP MODE
    // =========================

    WiFi.softAP(ap_ssid, ap_password);

    IPAddress IP = WiFi.softAPIP();

    Serial.println(IP);

    // =========================
    // LCD STATUS
    // =========================

    gfx->fillScreen(BLACK);

    gfx->setCursor(20, 30);
    gfx->println("HOTSPOT ACTIVE");

    gfx->setCursor(20, 70);
    gfx->println(ap_ssid);

    gfx->setCursor(20, 120);
    gfx->println("OPEN:");

    gfx->setCursor(20, 160);
    gfx->println(IP);

    gfx->setCursor(20, 220);
    gfx->println("OTA READY");

    // =========================
    // WEB PAGE
    // =========================

    server.on("/", HTTP_GET,
    [](AsyncWebServerRequest *request)
    {
        request->send_P(
            200,
            "text/html",
            index_html
        );
    });

    // =========================
    // OTA
    // =========================

    AsyncElegantOTA.begin(&server);

    // =========================
    // START SERVER
    // =========================

    server.begin();

    Serial.println("Server Started");
}

// ======================================================
// LOOP
// ======================================================

void loop()
{
    updateCyberUI();
}