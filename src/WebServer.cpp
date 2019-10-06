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

static const char NAV_BAR[] PROGMEM = 
    "<nav class='navbar navbar-expand-sm bg-dark navbar-dark fixed-top'>"
    "<a class='navbar-brand' href='index.html'>Weather Station</a>"
    "<ul class='navbar-nav'>"
    "<li class='nav-item'>"
    "<a class='nav-link' href='settings.html'>Settings</a>"
    "</li>"
    "<li class='nav-item'>"
    "<a class='nav-link' href='weatherSettings.html'>Weather</a>"
    "</li>"
    "<li class='nav-item'>"
    "<a class='nav-link' href='thingSpeakSettings.html'>ThingSpeak</a>"
    "</li>"
    "<li class='nav-item'>"
    "<a class='nav-link' href='mqttSettings.html'>MQTT</a>"
    "</li>"
    "<li class='nav-item'>"
    "<a class='nav-link' href='printMonitorSettings.html'>OctoPrint Monitor</a>"
    "</li>"
    "<li class='nav-item'>"
    "<a class='nav-link' href='screenGrab.html'>Screengrab</a>"
    "</nav>";

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
        request->send(SPIFFS, "/index.html", String(), false, tokenProcessor);
    });

    server.on("/screenGrab.html", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/screenGrab.html", String(), false, tokenProcessor);
    });

    server.on("/settings.html", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/settings.html", String(), false, tokenProcessor);
    });

    server.on("/weatherSettings.html", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/weatherSettings.html", String(), false, tokenProcessor);
    });

    server.on("/thingSpeakSettings.html", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/thingSpeakSettings.html", String(), false, tokenProcessor);
    });

    server.on("/mqttSettings.html", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/mqttSettings.html", String(), false, tokenProcessor);
    });

    server.on("/printMonitorSettings.html", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/printMonitorSettings.html", String(), false, tokenProcessor);
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

    server.on("/updateMqttSettings.html", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        handleMQTTSettings(request);
        request->redirect("/index.html");
    });

    server.on("/updateTimings.html", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        handleUpdateTimings(request);
        request->redirect("/index.html");
    });

    server.on("/updateClockSettings.html", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        handleUpdateClockSettings(request);
        request->redirect("/index.html");
    });

    server.on("/updatePrintMonitorSettings.html", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        handleUpdatePrintMonitorSettings(request);
        request->redirect("/index.html");
    });

    server.on("/resetSettings.html", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        handleResetSettings(request);
        request->redirect("/index.html");
    });

    server.on("/forgetWiFi.html", HTTP_GET, [](AsyncWebServerRequest *request)
    {
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

    DynamicJsonDocument jsonDoc(512);   // TODO

    jsonDoc["type"] = "sensorReadings";

    JsonObject readings = jsonDoc.createNestedObject("readings");
    readings["temp"] = temp;
    readings["humidity"] = humidity;
    readings["pressure"] = pressure;
    readings["metric"] = settingsManager->getDisplayMetric();

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
    weather["metric"] = settingsManager->getDisplayMetric();

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
    jsonDoc["metric"] = settingsManager->getDisplayMetric();

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

String WebServer::tokenProcessor(const String& token)
{
    if(token == "NAVBAR")
    {
        return FPSTR(NAV_BAR);
    }
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
    if(token == "DISPLAY4CHECKED")
    {
        if(settingsManager->getDisplayMode() == DisplayMode_4)
        {
            return "Checked";
        }
    }
     if(token == "DISPLAYMETRIC")
    {
        if(settingsManager->getDisplayMetric())
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
    if(token == "THINGSPEAKENABLED")
    {
        if(settingsManager->getThingSpeakEnabled() == true)
        {
            return "Checked";
        }
    }
    if(token == "PRINTMONITORINTERVAL")    
    {
        return String(settingsManager->getPrintMonitorInterval() / SECONDS_MULT);
    }
    if(token == "MQTTPUBLISHINTERVAL")    
    {
        return String(settingsManager->getMqttPublishInterval() / SECONDS_MULT);
    }    
    if(token == "MQTTENABLED")
    {
        if(settingsManager->getMqttEnabled() == true)
        {
            return "Checked";
        }
    }
    if(token == "MQTTBROKERURL")
    {
        return settingsManager->getMqttBroker();
    }
    if(token == "MQTTUSERNAME")
    {
        return settingsManager->getMqttUsername();
    }
    if(token == "MQTTPASSWORD")
    {
        return settingsManager->getMqttPassword();
    }
    if(token == "MQTTCLIENTID")
    {
        return settingsManager->getMqttClientId();
    }    
    if(token == "MQTTTEMPTOPIC")
    {
        return settingsManager->getMqttTempTopic();
    }
    if(token == "MQTTHUMIDITYTOPIC")
    {
        return settingsManager->getMqttHumidityTopic();
    }
    if(token == "MQTTPRESSURETOPIC")
    {
        return settingsManager->getMqttPressureTopic();
    }  
    if(token == "MQTTDISPLAYTOPIC")
    {
        return settingsManager->getMqttDisplayTopic();
    }   
    if(token == "MQTTPORT")
    {
        return String(settingsManager->getMqttPort());
    }
    if(token == "UTCOFFSET")
    {
        return String(settingsManager->getUtcOffset() / 3600.0f);
    }
    if(token == "BRIGHTNESS")
    {
        return String(settingsManager->getDisplayBrightness());
    }
    if(token == "PRINTMONITORENABLED")
    {
        if(settingsManager->getOctoPrintEnabled() == true)
        {
            return "Checked";
        }
    }
    if(token == "PRINTMONITORURL")
    {
        return String(settingsManager->getOctoPrintAddress());
    }
    if(token == "PRINTMONITOPORT")
    {
        return String(settingsManager->getOctoPrintPort());
    }
    if(token == "PRINTMONITORUSERNAME")
    {
        return String(settingsManager->getOctoPrintUsername());
    }
    if(token == "PRINTMONITORPASSWORD")
    {
        return String(settingsManager->getOctoPrintPassword());
    }
    if(token == "PRINTMONITORAPIKEY")
    {
        return String(settingsManager->getOctoPrintAPIKey());
    }
    if(token == "PRINTMONITORDISPLAYNAME")
    {
        return String(settingsManager->getOctoPrintDisplayName());
    }    

    return String();
}

void WebServer::handleUpdateWeatherSettings(AsyncWebServerRequest* request)
{
    if(request->hasParam("openWeatherLocation"))
    {
        AsyncWebParameter* p = request->getParam("openWeatherLocation");
        settingsManager->setOpenWeatherlocationID(p->value());
    }
    if(request->hasParam("openWeatherApiKey"))
    {
        AsyncWebParameter* p = request->getParam("openWeatherApiKey");
        settingsManager->setOpenWeatherApiKey(p->value());
    }
    if(request->hasParam("displayMetric"))
    {        
        settingsManager->setDisplayMetric(true);
    }
    else
    {
        settingsManager->setDisplayMetric(false);
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
    if(request->hasParam("thingSpeakEnabled"))
    {        
        settingsManager->setThingSpeakEnabled(true);
    }
    else
    {
        settingsManager->setThingSpeakEnabled(false);
    }    
}

void WebServer::handleMQTTSettings(AsyncWebServerRequest* request)
{
    if(request->hasParam("mqttEnabled"))
    {        
        settingsManager->setMqttEnabled(true);
    }
    else
    {
        settingsManager->setMqttEnabled(false);
    }

    if(request->hasParam("mqttBroker"))
    {
        AsyncWebParameter* p = request->getParam("mqttBroker");
        settingsManager->setMqttBroker(p->value());
    }
    if(request->hasParam("mqttPort"))
    {
        AsyncWebParameter* p = request->getParam("mqttPort");
        settingsManager->setMqttPort(p->value().toInt());
    }
    if(request->hasParam("mqttUsername"))
    {
        AsyncWebParameter* p = request->getParam("mqttUsername");
        settingsManager->setMqttUsername(p->value());
    }
    if(request->hasParam("mqttPassword"))
    {
        AsyncWebParameter* p = request->getParam("mqttPassword");
        settingsManager->setMqttPassword(p->value());
    }
    if(request->hasParam("mqttClientId"))
    {
        AsyncWebParameter* p = request->getParam("mqttClientId");
        settingsManager->setMqttClientId(p->value());
    }
    if(request->hasParam("mqttTempTopic"))
    {
        AsyncWebParameter* p = request->getParam("mqttTempTopic");
        settingsManager->setMqttTempTopic(p->value());
    }
    if(request->hasParam("mqttHumidityTopic"))
    {
        AsyncWebParameter* p = request->getParam("mqttHumidityTopic");
        settingsManager->setMqttHumidityTopic(p->value());
    }
    if(request->hasParam("mqttPressureTopic"))
    {
        AsyncWebParameter* p = request->getParam("mqttPressureTopic");
        settingsManager->setMqttPressureTopic(p->value());
    }
    if(request->hasParam("mqttDisplayTopic"))
    {
        AsyncWebParameter* p = request->getParam("mqttDisplayTopic");
        settingsManager->setMqttDisplayTopic(p->value());
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
        if(p->value() == "display4")
        {
            settingsManager->setDisplayMode(DisplayMode_4);
        }

        //Serial.println("Got display mode of: " + p->value());
    }
    if(request->hasParam("brightness"))
    {
        AsyncWebParameter* p = request->getParam("brightness");
        settingsManager->setDisplayBrightness(p->value().toInt());
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
    if(request->hasParam("mqttPublishInterval"))
    {
        AsyncWebParameter* p = request->getParam("mqttPublishInterval");
        settingsManager->setMqttPublishInterval(p->value().toInt() * SECONDS_MULT);
    }
    if(request->hasParam("printMonitorInterval"))
    {
        AsyncWebParameter* p = request->getParam("printMonitorInterval");
        settingsManager->setPrintMonitorInterval(p->value().toInt() * SECONDS_MULT);
    }
    
}

void WebServer::handleUpdateClockSettings(AsyncWebServerRequest* request)
{
    if(request->hasParam("utcOffset"))
    {
        AsyncWebParameter* p = request->getParam("utcOffset");
        settingsManager->setUtcOffset(p->value().toFloat() * 3600.0f);
    }
}

void WebServer::handleUpdatePrintMonitorSettings(AsyncWebServerRequest* request)
{
    if(request->hasParam("printMonitorEnabled"))
    {        
        settingsManager->setOctoPrintEnabled(true);
    }
    else
    {
        settingsManager->setOctoPrintEnabled(false);
    }

    if(request->hasParam("octoPrintUrl"))
    {
        AsyncWebParameter* p = request->getParam("octoPrintUrl");
        settingsManager->setOctoPrintAddress(p->value());
    }
    if(request->hasParam("octoPrintPort"))
    {
        AsyncWebParameter* p = request->getParam("octoPrintPort");
        settingsManager->setOctoPrintPort(p->value().toInt());
    }
    if(request->hasParam("octoPrintUsername"))
    {
        AsyncWebParameter* p = request->getParam("octoPrintUsername");
        settingsManager->setOctoPrintUsername(p->value());
    }
    if(request->hasParam("octoPrintPassword"))
    {
        AsyncWebParameter* p = request->getParam("octoPrintPassword");
        settingsManager->setOctoPrintPassword(p->value());
    }
    if(request->hasParam("octoPrintAPIKey"))
    {
        AsyncWebParameter* p = request->getParam("octoPrintAPIKey");
        settingsManager->setOctoPrintAPIKey(p->value());
    }
    if(request->hasParam("octoPrintDisplayName"))
    {
        AsyncWebParameter* p = request->getParam("octoPrintDisplayName");
        settingsManager->setOctoPrintDisplayName(p->value());
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
