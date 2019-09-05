#include <ArduinoJson.h>
#include "WebServer.h"

// globals
AsyncWebServer server(80);
AsyncWebSocket webSocket("/ws");
AsyncEventSource events("/events");

// methods

void WebServer::init()
{
    webSocket.onEvent(onEvent);
    server.addHandler(&webSocket);
    server.addHandler(&events);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/index.html");
    });

    server.on("/js/station.js", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/js/station.js");
    });

    server.serveStatic("/img", SPIFFS, "/img/");

    server.begin();
}

AsyncWebServer* WebServer::getServer()
{
    return &server;
}

void WebServer::updateSensorReadings(float temp, float humidity, float pressure)
{
    if(webSocket.count() > 0)
    {
        String output;

        Serial.println("Client connected, sending sensor readings");

        const size_t capacity = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3);
        DynamicJsonDocument jsonDoc(capacity);

        jsonDoc["type"] = "sensorReadings";

        JsonObject readings = jsonDoc.createNestedObject("readings");
        readings["temp"] = temp;
        readings["humidity"] = humidity;
        readings["pressure"] = pressure;

        serializeJson(jsonDoc, Serial);
        Serial.println();
        
        serializeJson(jsonDoc, output);
        webSocket.textAll(output);
    }
}

void WebServer::updateCurrentWeather(OpenWeatherMapCurrentData& currentWeather)
{

}
    
void WebServer::updateForecastWeather(OpenWeatherMapForecastData* forecastWeather, int numForecasts)
{
    
}

void WebServer::onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len)
{
    if(type == WS_EVT_CONNECT)
    {
        Serial.println("Websocket connect");
    }
    if(type == WS_EVT_DISCONNECT)
    {
        Serial.println("Websocket disconnect");
    }
    if(type == WS_EVT_DATA)
    {
        Serial.println("Websocket data");
 
        AwsFrameInfo * info = (AwsFrameInfo*)arg;
        String msg = "";
        
        if(info->final && info->index == 0 && info->len == len)
        {
            //the whole message is in a single frame and we got all of it's data
            Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT)?"text":"binary", info->len);

            if(info->opcode == WS_TEXT)
            {
                for(size_t i=0; i < info->len; i++) 
                {
                    msg += (char) data[i];
                }
            }
            else 
            {
                char buff[3];
                for(size_t i=0; i < info->len; i++)
                {
                    sprintf(buff, "%02x ", (uint8_t) data[i]);
                    msg += buff ;
                }
            }
        }
        Serial.printf("%s\n",msg.c_str());

        Serial.println();
    }    
}
