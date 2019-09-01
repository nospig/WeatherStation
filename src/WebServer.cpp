#include "WebServer.h"

AsyncWebServer server(80);
AsyncWebSocket webSocket("/ws");
AsyncEventSource events("/events");

void WebServer::init()
{
    //webSocket.onEvent(this->onEvent);
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/test_file.html");
    });

    server.begin();
}

AsyncWebServer* WebServer::getServer()
{
    return &server;
}

void WebServer::onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len)
{
  //Handle WebSocket event
}
