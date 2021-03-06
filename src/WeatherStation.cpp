#include <Arduino.h>
#include <ArduinoOTA.h>
#include <TaskScheduler.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESPAsyncWifiManager.h>
#include "OpenWeatherMapCurrent.h"
#include "OpenWeatherMapDaily.h"
#include "SettingsManager.h"
#include "Settings.h"
#include "WeatherStation.h"
#include "DisplayTFT.h"
#include "WebServer.h"
#include "ThingSpeakReporter.h"
#include "BMEReader.h"
#include "MQTTManager.h"
#include "OctoPrintMonitor.h"
#include <FS.h>

// globals
Scheduler taskScheduler;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
WebServer webServer;
ThingSpeakReporter thingSpeakReporter;
DisplayBase* display;
OpenWeatherMapCurrent currentWeatherClient;
OpenWeatherMapDaily forecastWeatherClient(NUM_FORECASTS);
SettingsManager settingsManager;
DNSServer dns;
BMEReader bmeReader;
MQTTManager mqttManager;
OctoPrintMonitor octoPrintMonitor;

float sensorTemp = 0, sensorHumidity = 0, sensorPressure = 0;

// tasks
Task connectWifi(0, TASK_ONCE, &connectWifiCallback);
Task getTime(TIME_FETCH_INTERVAL, TASK_FOREVER, &getTimeCallback);
Task readSensors(60*SECONDS_MULT, TASK_FOREVER, &readSensorsCallback);
Task getCurrentWeather(60*SECONDS_MULT, TASK_FOREVER, &getCurrentWeatherCallback);
Task getForecastWeather(60*SECONDS_MULT, TASK_FOREVER, &getWeatherForecastCallback);
Task updateThingSpeak(60*SECONDS_MULT, TASK_FOREVER, &updateThingSpeakCallback);
Task updateWiFiStrength(WIFI_STRENGTH_INTERVAL, TASK_FOREVER, &updateWifiStrengthCallback);
Task checkScreenGrabRequested(SCREENGRAB_INTERVAL, TASK_FOREVER, &checkScreenGrabCallback);
Task mqttPublish(5*MINUTES_MULT, TASK_FOREVER, &mqttPublishCallback);
Task octoPrintUpdate(5*MINUTES_MULT, TASK_FOREVER, &updatePrinterMonitorCallback);

// task callbacks

// time

void getTimeCallback()
{
    timeClient.update();
    display->drawCurrentTime(timeClient.getEpochTime());
}

// sensors

void readSensorsCallback()
{
    if(bmeReader.isActive())
    {
        sensorTemp = bmeReader.readTemp();
        sensorHumidity = bmeReader.readHumidity();
        sensorPressure = bmeReader.readPressurehPA();

        display->drawSensorReadings(sensorTemp, sensorHumidity, sensorPressure);
        webServer.updateSensorReadings(sensorTemp, sensorHumidity, sensorPressure);
        
        // only enable thing speak and mqtt after some data recorded
        updateThingSpeak.enableIfNot();
        mqttPublish.enableIfNot();
    }
}

// ThingSpeak

void updateThingSpeakCallback()
{
    // just send the latest sensor saved readings, no need to update again, avoid chances of updating the sensor too soon
    if(settingsManager.getThingSpeakApiKey() != "" && settingsManager.getThingSpeakEnabled())
    {
        thingSpeakReporter.sendSensorReadings(sensorTemp, sensorHumidity, sensorPressure);
    }
}

// weather

void getCurrentWeatherCallback()
{
    //Serial.println("getCurrentWeatherCallback");

    if(settingsManager.getOpenWeatherApiKey() != "" && settingsManager.getOpenWeatherlocationID() != "")
    {
        currentWeatherClient.updateById(settingsManager.getOpenWeatherApiKey(), settingsManager.getOpenWeatherlocationID());
        display->drawCurrentWeather(currentWeatherClient.getCurrentData());
        webServer.updateCurrentWeather(currentWeatherClient.getCurrentData());
    }
}

void getWeatherForecastCallback()
{
    //Serial.println("Get forecast weather");

    if(settingsManager.getOpenWeatherApiKey() != "" && settingsManager.getOpenWeatherlocationID() != "")
    {
        forecastWeatherClient.updateById(settingsManager.getOpenWeatherApiKey(), settingsManager.getOpenWeatherlocationID());
        display->drawForecastWeather(forecastWeatherClient.isValidData(), forecastWeatherClient.getDailyForecasts(), forecastWeatherClient.getForecastCount());
        webServer.updateForecastWeather(forecastWeatherClient.isValidData(), forecastWeatherClient.getDailyForecasts(), forecastWeatherClient.getForecastCount());
    }
}

// MQTT

void mqttPublishCallback()
{
    // just send latest readings
    mqttManager.updateSensorReadings(sensorTemp, sensorHumidity, sensorPressure);
}

// Printer monitor

void updatePrinterMonitorCallback()
{
    //Serial.println("updatePrinterMonitorCallback");

    if(settingsManager.getOctoPrintEnabled())
    {
        octoPrintMonitor.update();
    }
    
    display->drawOctoPrintStatus(octoPrintMonitor.getCurrentData(), settingsManager.getOctoPrintDisplayName(), settingsManager.getOctoPrintEnabled());
}

// wifi

void connectWifiCallback()
{
    AsyncWiFiManager wifiManager(webServer.getServer(), &dns);

    settingsManager.init();

    display->setDisplayBrightness(settingsManager.getDisplayBrightness());
    display->setDisplayMetric(settingsManager.getDisplayMetric());
    display->drawStartupDisplay();

    wifiManager.autoConnect("Weather Station");

    setupOtaUpdates();

    Serial.println(WiFi.localIP());

    webServer.init(&settingsManager);
    thingSpeakReporter.init(&settingsManager);
    bmeReader.init(BME_SDA, BME_SCL, BME_ADDRESS);
    mqttManager.init(&settingsManager);
    mqttManager.setSubscribeCallback(mqttSubscribeCallback);

    octoPrintMonitor.init(settingsManager.getOctoPrintAddress(), settingsManager.getOctoPrintPort(), 
                          settingsManager.getOctoPrintAPIKey(), settingsManager.getOctoPrintUsername(), settingsManager.getOctoPrintPassword());
    timeClient.setTimeOffset(settingsManager.getUtcOffset());

    currentWeatherClient.setMetric(settingsManager.getDisplayMetric());
    forecastWeatherClient.setMetric(settingsManager.getDisplayMetric());
    settingsManager.setSettingsChangedCallback(settingsChangedCallback);

    delay(WIFI_CONNECTING_DELAY);
    display->setDisplayMode(settingsManager.getDisplayMode());
    display->setDisplayBrightness(settingsManager.getDisplayBrightness());
    display->setClockFormat(settingsManager.getClockFormat());
    display->setDateFormat(settingsManager.getDateFormat());
    display->clearDisplay();

    if(!settingsManager.getSetupDone())
    {
        display->drawNotSetupDisplay();
    }

    taskScheduler.addTask(getTime);
    taskScheduler.addTask(getCurrentWeather);
    taskScheduler.addTask(getForecastWeather);
    taskScheduler.addTask(readSensors);
    taskScheduler.addTask(updateThingSpeak);
    taskScheduler.addTask(updateWiFiStrength);
    taskScheduler.addTask(checkScreenGrabRequested);
    taskScheduler.addTask(mqttPublish);
    taskScheduler.addTask(octoPrintUpdate);

    // timings
    getCurrentWeather.setInterval(settingsManager.getCurrentWeatherInterval());
    getForecastWeather.setInterval(settingsManager.getForecastWeatherInterval());
    readSensors.setInterval(settingsManager.getSensorReadingInterval());
    updateThingSpeak.setInterval(settingsManager.getThingSpeakReportingInterval());
    mqttPublish.setInterval(settingsManager.getMqttPublishInterval());
    octoPrintUpdate.setInterval(settingsManager.getPrintMonitorInterval());

    updateThingSpeak.disable(); 
    mqttPublish.disable();
    getTime.enable();
    readSensors.enable();
    updateWiFiStrength.enable();
    checkScreenGrabRequested.enable();

    switch(settingsManager.getDisplayMode())
    {
        case DisplayMode_PrintMonitor:
            octoPrintUpdate.enable();
            getCurrentWeather.disable();
            getForecastWeather.disable();
        break;

        default:
            octoPrintUpdate.disable();
            getCurrentWeather.enable();
            getForecastWeather.enable();
            break;
    }
}

void updateWifiStrengthCallback()
{
    long wifiStrength = WiFi.RSSI();
    display->drawWiFiStrength(wifiStrength);
}

// settings

void settingsChangedCallback()
{
    if(settingsManager.getMqttReconnectRequired())
    {
        mqttManager.reconnect();
        settingsManager.resetMqttReconnectRequired();
    }

    timeClient.setTimeOffset(settingsManager.getUtcOffset());

    currentWeatherClient.setMetric(settingsManager.getDisplayMetric());
    forecastWeatherClient.setMetric(settingsManager.getDisplayMetric());

    octoPrintMonitor.updateSettings(settingsManager.getOctoPrintAddress(), settingsManager.getOctoPrintPort(), 
                                    settingsManager.getOctoPrintAPIKey(), settingsManager.getOctoPrintUsername(), settingsManager.getOctoPrintPassword());

    // best just to force a display clear when changing settings
    display->setDisplayMode(settingsManager.getDisplayMode());
    display->setDisplayBrightness(settingsManager.getDisplayBrightness());
    display->setDisplayMetric(settingsManager.getDisplayMetric());
    display->setClockFormat(settingsManager.getClockFormat());
    display->setDateFormat(settingsManager.getDateFormat());

    getCurrentWeather.setInterval(settingsManager.getCurrentWeatherInterval());
    getForecastWeather.setInterval(settingsManager.getForecastWeatherInterval());
    readSensors.setInterval(settingsManager.getSensorReadingInterval());
    updateThingSpeak.setInterval(settingsManager.getThingSpeakReportingInterval());
    mqttPublish.setInterval(settingsManager.getMqttPublishInterval());
    octoPrintUpdate.setInterval(settingsManager.getPrintMonitorInterval());

    switch(settingsManager.getDisplayMode())
    {
        case DisplayMode_PrintMonitor:
            octoPrintUpdate.enableIfNot();
            getCurrentWeather.disable();
            getForecastWeather.disable();
            octoPrintUpdate.forceNextIteration();
        break;

        default:
            octoPrintUpdate.disable();
            getCurrentWeather.enableIfNot();
            getForecastWeather.enableIfNot();
            getCurrentWeather.forceNextIteration();
            getForecastWeather.forceNextIteration();
            break;
    }

    getTime.forceNextIteration();
    readSensors.forceNextIteration();
    updateWiFiStrength.forceNextIteration();
    updateThingSpeak.forceNextIteration();
    mqttPublish.forceNextIteration();    
}

// screen grabs

void checkScreenGrabCallback()
{
    if(webServer.screenGrabRequested())
    {
        display->serveScreenShot();
        webServer.clearScreenGrabRequest();
    }
}

// mqtt

void mqttSubscribeCallback(const char* topic, const char *payload)
{
    if(!strcmp(topic, settingsManager.getMqttDisplayTopic().c_str()))
    {
        if(!strcmp(payload, "on"))
        {
            //Serial.println("Display on");
            display->setDisplayEnabled(true);
        }
        if(!strcmp(payload, "off"))
        {
            //Serial.println("Display off");
            display->setDisplayEnabled(false);
        }
    }
}

// basic setup and loop

void setup() 
{
    Serial.begin(115200);

    SPIFFS.begin();

    //display = new DisplaySerial();
    display = new DisplayTFT();

    currentWeatherClient.setLanguage("en");
    forecastWeatherClient.setLanguage("en");

    WiFi.hostname("Weather-Station");

    taskScheduler.startNow(); 
    taskScheduler.addTask(connectWifi);
    connectWifi.enable();
}

void loop() 
{
    taskScheduler.execute();
    ArduinoOTA.handle();
}

// OTA, not through webserver for now

void setupOtaUpdates()
{
    // set MD5 hash password here
    // add a file to the project called uploadPassword.ini
    // make it's contents
    // [password]
    // upload_password = password
    // where password is plain text
    ArduinoOTA.setPasswordHash(OTA_MD5_PASSWORD_HASH);

    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
        {
            Serial.println("Start updating firmware.");
        }
        else
        {
            SPIFFS.end();
            Serial.println("Start updating file system.");
        }
    });

    ArduinoOTA.onEnd([]() 
    {
        Serial.println("\nEnd");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });

    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
        {
            Serial.println("Auth Failed");
        }
        else if (error == OTA_BEGIN_ERROR)
        {
            Serial.println("Begin Failed");
        }
        else if (error == OTA_CONNECT_ERROR)
        {
            Serial.println("Connect Failed");
        }
        else if (error == OTA_RECEIVE_ERROR)
        {
            Serial.println("Receive Failed");
        }
        else if (error == OTA_END_ERROR)
        {
            Serial.println("End Failed");
        }
    });

    ArduinoOTA.begin();
}