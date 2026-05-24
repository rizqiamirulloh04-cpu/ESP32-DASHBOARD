#pragma once

#include <Arduino.h>

#include <ESPAsyncWebServer.h>

extern AsyncWebServer server;

void initWebDashboard();

void handleWebDashboard();