#include <Arduino.h>


extern AsyncWebServer server;

AsyncWebSocket ws("/ws");

// ======================================================
// HTML
// ======================================================

const char dashboard_html[] PROGMEM = R"rawliteral(

<!DOCTYPE html>
<html>

<head>

<meta name="viewport" content="width=device-width, initial-scale=1">

<title>Cyber Dashboard</title>

<style>

body{
    margin:0;
    padding:0;

    background:#050505;
    color:white;

    font-family:Arial;

    text-align:center;
}

.card{

    width:320px;

    margin:auto;

    margin-top:40px;

    background:#111;

    border-radius:24px;

    padding:24px;
}

.value{
    font-size:48px;
    color:cyan;
}

.status{
    margin-top:20px;
    color:lime;
}

</style>

</head>

<body>

<div class="card">

<h1>CYBER DASHBOARD</h1>

<div class="value" id="speed">
0
</div>

<p>SPEED</p>

<div class="status" id="status">
CONNECTING...
</div>

</div>

<script>

let gateway = `ws://${window.location.hostname}/ws`;

let websocket;

function initWebSocket()
{
    websocket = new WebSocket(gateway);

    websocket.onopen = () =>
    {
        document.getElementById("status").innerHTML = "CONNECTED";
    };

    websocket.onclose = () =>
    {
        document.getElementById("status").innerHTML = "DISCONNECTED";

        setTimeout(initWebSocket, 2000);
    };

    websocket.onmessage = (event) =>
    {
        document.getElementById("speed").innerHTML = event.data;
    };
}

window.addEventListener('load', initWebSocket);

</script>

</body>
</html>

)rawliteral";

// ======================================================
// WEBSOCKET EVENT
// ======================================================

void onEvent(
    AsyncWebSocket *server,
    AsyncWebSocketClient *client,
    AwsEventType type,
    void *arg,
    uint8_t *data,
    size_t len)
{
    if(type == WS_EVT_CONNECT)
    {
        Serial.println("WebSocket Client Connected");
    }
}

// ======================================================
// INIT
// ======================================================

void initWebDashboard()
{
    ws.onEvent(onEvent);

    server.addHandler(&ws);

    server.on("/dashboard", HTTP_GET,
    [](AsyncWebServerRequest *request)
    {
        request->send_P(
            200,
            "text/html",
            dashboard_html
        );
    });
}

// ======================================================
// UPDATE
// ======================================================

void handleWebDashboard()
{
    static uint32_t last = 0;

    if(millis() - last > 500)
    {
        last = millis();

        static int speed = 0;

        speed += 5;

        if(speed > 180)
        {
            speed = 0;
        }

        ws.textAll(String(speed));
    }
}