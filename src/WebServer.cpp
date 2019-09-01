#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include "WebServer.h"

AsyncWebServer server(80);

void WebServer::init()
{
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/test_file.html");
    });

    server.begin();
}