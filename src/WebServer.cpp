#include <Arduino.h>
#include <ArduinoJson.h>
#include "WebServer.h"

// globals
AsyncWebServer server(80);
AsyncWebSocket webSocket("/ws");
AsyncEventSource events("/events");

String WebServer::currentSensorJson = "";
String WebServer::currentWeatherJson = "";
String WebServer::forecastWeatherJson ="";     

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

    server.on("/css/station.css", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/css/station.css");
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
    String output;

    const size_t capacity = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3);
    DynamicJsonDocument jsonDoc(capacity);

    jsonDoc["type"] = "sensorReadings";

    JsonObject readings = jsonDoc.createNestedObject("readings");
    readings["temp"] = temp;
    readings["humidity"] = humidity;
    readings["pressure"] = pressure;

    serializeJson(jsonDoc, output);
    currentSensorJson = output;

    if(webSocket.count() > 0)
    {
        Serial.println("Sending sensor readings to cilent");
        webSocket.textAll(output);
    }
}

void WebServer::updateCurrentWeather(OpenWeatherMapCurrentData* currentWeather)
{
    String output;

    const size_t capacity = 1024;   // TODO
    DynamicJsonDocument jsonDoc(capacity);

    jsonDoc["type"] = "currentWeather";

    JsonObject weather = jsonDoc.createNestedObject("currentReadings");
    weather["temp"] = currentWeather->temp;
    weather["humidity"] = currentWeather->humidity;
    weather["windSpeed"] = currentWeather->windSpeed;
    weather["windDirection"] = currentWeather->windDeg;
    weather["description"] = currentWeather->description;
    weather["time"] = currentWeather->observationTime;

    serializeJson(jsonDoc, output);
    currentWeatherJson = output;

    if(webSocket.count() > 0)
    {
        Serial.println("Sending current internet weather to cilent");
        webSocket.textAll(output);
    }
}
    
void WebServer::updateForecastWeather(OpenWeatherMapDailyData* forecastWeather, int forecastCount)
{
    
}

void WebServer::updateClientOnConnect()
{
    Serial.println("Client connected, sending any available data.");

    if(currentSensorJson.length() != 0)
    {
        webSocket.textAll(currentSensorJson);
    }
    if(currentWeatherJson.length() != 0)
    {
        webSocket.textAll(currentWeatherJson);
    }
    if(forecastWeatherJson.length() != 0)
    {
        webSocket.textAll(forecastWeatherJson);
    }
}

void WebServer::onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len)
{
    if(type == WS_EVT_CONNECT)
    {
        Serial.println("Websocket connect");
        updateClientOnConnect();
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
