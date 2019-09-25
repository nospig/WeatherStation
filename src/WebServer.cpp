#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncWiFiManager.h>
#include "WebServer.h"

// globals
AsyncWebServer server(80);
AsyncWebSocket webSocket("/ws");
AsyncEventSource events("/events");

String WebServer::currentSensorJson = "";
String WebServer::currentWeatherJson = "";
String WebServer::forecastWeatherJson ="";     
bool WebServer::screenGrabRequest = false;

SettingsManager* WebServer::settingsManager;    

// methods

void WebServer::init(SettingsManager* settingsManager)
{
    this->settingsManager = settingsManager;
    
    webSocket.onEvent(onEvent);
    server.addHandler(&webSocket);
    server.addHandler(&events);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/index.html");
    });

    server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/index.html", String(), false, indexProcessor);
    });

    server.on("/screenGrab.html", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/screenGrab.html");
    });

    server.on("/settings.html", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/settings.html", String(), false, settingsProcessor);
    });

    server.on("/updateWeatherSettings.html", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        handleUpdateWeatherSettings(request);
        request->redirect("/index.html");
    });

    server.on("/updateThingSpeak.html", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        handleUpdateThingSpeakSettings(request);
        request->redirect("/index.html");
    });

    server.on("/updateDisplaySettings.html", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        handleUpdateDisplaySettings(request);
        request->redirect("/index.html");
    });

    server.on("/updateTimings.html", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        handleUpdateTimings(request);
        request->redirect("/index.html");
    });

    server.on("/resetSettings.html", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        handleResetSettings(request);
        //Serial.println("Reset settings");
        request->redirect("/index.html");
    });

    server.on("/forgetWiFi.html", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        //Serial.println("Forget WiFi");
        request->redirect("/index.html");

        handleForgetWiFi(request);
    });

    server.on("/takeScreenGrab.html", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        handleScreenGrab(request);
        request->redirect("/screenGrab.html");
    });

    server.serveStatic("/img", SPIFFS, "/img/");
    server.serveStatic("/js", SPIFFS, "/js/");
    server.serveStatic("/css", SPIFFS, "/css/");

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
        //Serial.println("Sending sensor readings to cilent");
        webSocket.textAll(output);
    }
}

void WebServer::updateCurrentWeather(OpenWeatherMapCurrentData* currentWeather)
{
    if(!currentWeather->validData)
    {
        currentWeatherJson = "";
        return;
    }

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
        //Serial.println("Sending current internet weather to cilent");
        webSocket.textAll(output);
    }
}
    
void WebServer::updateForecastWeather(bool validData, OpenWeatherMapDailyData* forecastWeather, int forecastCount)
{
    if(!validData)
    {
        forecastWeatherJson = "";
        return;
    }

    String output;

    const size_t capacity = 1024;   // TODO
    DynamicJsonDocument jsonDoc(capacity);

    jsonDoc["type"] = "weatherForecast";
    jsonDoc["count"] = forecastCount;

    JsonArray list = jsonDoc.createNestedArray("list");

    for(int i=0; i<forecastCount; i++)
    {
        JsonObject day = list.createNestedObject();

        day["time"] = forecastWeather[i].time;
        day["description"] = forecastWeather[i].description;
        day["tempMin"] = forecastWeather[i].tempMin;
        day["tempMax"] = forecastWeather[i].tempMax;
    }

    serializeJson(jsonDoc, output);
    forecastWeatherJson = output;

    if(webSocket.count() > 0)
    {
        webSocket.textAll(output);
    }
}

void WebServer::updateClientOnConnect()
{
    //Serial.println("Client connected, sending any available data.");

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
        //Serial.println("Websocket connect");
        updateClientOnConnect();
    }
}

String WebServer::settingsProcessor(const String& token)
{
    if(token == "WEATHERLOCATIONKEY")    
    {
        return settingsManager->getOpenWeatherlocationID();
    }
    if(token == "WEATHERAPIKEY")    
    {
        return settingsManager->getOpenWeatherApiKey();
    }
    if(token == "THINGSPEAKCHANNELKEY")    
    {
        return String(settingsManager->getThingSpeakChannelID());
    }
    if(token == "THINGSPEAKAPIKEY")    
    {
        return settingsManager->getThingSpeakApiKey();
    }
    if(token == "DISPLAY1CHECKED")
    {
        if(settingsManager->getDisplayMode() == DisplayMode_1)
        {
            return "Checked";
        }
    }
    if(token == "DISPLAY2CHECKED")
    {
        if(settingsManager->getDisplayMode() == DisplayMode_2)
        {
            return "Checked";
        }
    }
    if(token == "DISPLAY3CHECKED")
    {
        if(settingsManager->getDisplayMode() == DisplayMode_3)
        {
            return "Checked";
        }
    }
    if(token == "CURRENTWEATHERINTERVAL")    
    {
        return String(settingsManager->getCurrentWeatherInterval() / SECONDS_MULT);
    }
    if(token == "WEATHERFORECASTINTERVAL")    
    {
        return String(settingsManager->getForecastWeatherInterval() / SECONDS_MULT);
    }
    if(token == "SENSORREADINGINTERVAL")    
    {
        return String(settingsManager->getSensorReadingInterval() / SECONDS_MULT);
    }
    if(token == "THINGSPEAKREPORTINGINTERVAL")    
    {
        return String(settingsManager->getThingSpeakReportingInterval() / SECONDS_MULT);
    }

    return String();
}

String WebServer::indexProcessor(const String& token)
{
    if(token == "THINGSPEAKLINK")
    {
        if(settingsManager->getThingSpeakChannelID() != 0)
        {
            String url = "https://thingspeak.com/channels/" + String(settingsManager->getThingSpeakChannelID());
            return "<li class='nav-item'><a class='nav-link' href='" + url + "'>ThingSpeak</a></li>";
        }
    }

    return String();
}

void WebServer::handleUpdateWeatherSettings(AsyncWebServerRequest* request)
{
    if(request->hasParam("openWeatherLocation"))
    {
        AsyncWebParameter* p = request->getParam("openWeatherLocation");
        settingsManager->setOpenWeatherlocationID(p->value());

        //Serial.println("Got weather location of: " + p->value());
    }
    if(request->hasParam("openWeatherApiKey"))
    {
        AsyncWebParameter* p = request->getParam("openWeatherApiKey");
        settingsManager->setOpenWeatherApiKey(p->value());

        //Serial.println("Got weather API key of: " + p->value());
    }
}

void WebServer::handleUpdateThingSpeakSettings(AsyncWebServerRequest* request)
{
    if(request->hasParam("thingSpeakApiKey"))
    {
        AsyncWebParameter* p = request->getParam("thingSpeakApiKey");
        settingsManager->setThingSpeakApiKey(p->value());

        //Serial.println("Got ThingSpeak API key of: " + p->value());
    }
    if(request->hasParam("thingSpeakChannel"))
    {
        AsyncWebParameter* p = request->getParam("thingSpeakChannel");
        settingsManager->setThingSpeakChannelID(p->value().toInt());

        //Serial.println("Got ThingSpeak channel ID of: " + p->value());
    }
}

void WebServer::handleUpdateDisplaySettings(AsyncWebServerRequest* request)
{
    if(request->hasParam("optdisplay"))
    {
        AsyncWebParameter* p = request->getParam("optdisplay");
        
        if(p->value() == "display1")
        {
            settingsManager->setDisplayMode(DisplayMode_1);
        }
        if(p->value() == "display2")
        {
            settingsManager->setDisplayMode(DisplayMode_2);
        }
        if(p->value() == "display3")
        {
            settingsManager->setDisplayMode(DisplayMode_3);
        }

        //Serial.println("Got display mode of: " + p->value());
    }
}

void WebServer::handleUpdateTimings(AsyncWebServerRequest* request)
{
    if(request->hasParam("currentWeatherInterval"))
    {
        AsyncWebParameter* p = request->getParam("currentWeatherInterval");
        settingsManager->setCurrentWeatherInterval(p->value().toInt() * SECONDS_MULT);
    }
    if(request->hasParam("forecastWeatherInterval"))
    {
        AsyncWebParameter* p = request->getParam("forecastWeatherInterval");
        settingsManager->setForecastWeatherInterval(p->value().toInt() * SECONDS_MULT);
    }
    if(request->hasParam("sensorReadingInterval"))
    {
        AsyncWebParameter* p = request->getParam("sensorReadingInterval");
        settingsManager->setSensorReadingInterval(p->value().toInt() * SECONDS_MULT);
    }
    if(request->hasParam("thingSpeakReportingInterval"))
    {
        AsyncWebParameter* p = request->getParam("thingSpeakReportingInterval");
        settingsManager->setThingSpeakReportingInterval(p->value().toInt() * SECONDS_MULT);
    }
}

void WebServer::handleForgetWiFi(AsyncWebServerRequest* request)
{
    DNSServer dns;
    AsyncWiFiManager wifiManager(getServer(), &dns);
    wifiManager.resetSettings();
    ESP.restart();
}

void WebServer::handleResetSettings(AsyncWebServerRequest* request)
{
    settingsManager->resetSettings();
}

void WebServer::handleScreenGrab(AsyncWebServerRequest* request)
{
    screenGrabRequest = true;
}

bool WebServer::screenGrabRequested()
{
    return screenGrabRequest;
}

void WebServer::clearScreenGrabRequest()
{
    screenGrabRequest = false;
}
