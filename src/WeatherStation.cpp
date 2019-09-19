#include <Arduino.h>
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
#include <FS.h>

// globals
Scheduler taskScheduler;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
WebServer webServer;
ThingSpeakReporter thingSpeakReporter;
DisplayBase* display;
OpenWeatherMapCurrent currentWeatherClient;
OpenWeatherMapDaily forecastWeatherClient(NUM_FORECASTS);
SettingsManager settingsManager;
DNSServer dns;
BMEReader bmeReader;
float sensorTemp = 0, sensorHumidity = 0, sensorPressure = 0;

// tasks
Task connectWifi(0, TASK_ONCE, &connectWifiCallback);
Task getTime(TIME_FETCH_INTERVAL, TASK_FOREVER, &getTimeCallback);
Task readSensors(SENSOR_READING_INTERVAL, TASK_FOREVER, &readSensorsCallback);
Task getCurrentWeather(CURRENT_WEATHER_INTERVAL, TASK_FOREVER, &getCurrentWeatherCallback);
Task getForecastWeather(FORECAST_WEATHER_INTERVAL, TASK_FOREVER, &getWeatherForecastCallback);
Task updateThingSpeak(THINGSPEAK_REPORTING_INTERVAL, TASK_FOREVER, &updateThingSpeakCallback);
Task updateWiFiStrength(WIFI_STRENGTH_INTERVAL, TASK_FOREVER, &updateWifiStrengthCallback);
Task checkSettingsChanged(SETTINGS_CHANGED_INTERVAL, TASK_FOREVER, &checkSettingsChangedCallback);
Task checkScreenGrabRequested(SCREENGRAB_INTERVAL, TASK_FOREVER, &checkScreenGrabCallback);


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
    //Serial.println("Read sensors now");

    if(bmeReader.isActive())
    {
        sensorTemp = bmeReader.readTemp();
        sensorHumidity = bmeReader.readHumidity();
        sensorPressure = bmeReader.readPressurehPA();

        display->drawSensorReadings(sensorTemp, sensorHumidity, sensorPressure);
        webServer.updateSensorReadings(sensorTemp, sensorHumidity, sensorPressure);

        // only enable thing speak after some data recorded
        updateThingSpeak.enableIfNot();
    }
}

// ThingSpeak

void updateThingSpeakCallback()
{
    //Serial.println("Update ThingSpeak");

    // just send the latest sensor saved readings, no need to update again, avoid chances of updating the sensor too soon
    if(settingsManager.getThingSpeakApiKey() != "")
    {
        thingSpeakReporter.sendSensorReadings(sensorTemp, sensorHumidity, sensorPressure);
    }
}

// weather

void getCurrentWeatherCallback()
{
    //Serial.println("Get current weather");
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

// display

// only call if all data is available

void changeDisplayMode(DisplayMode newMode)
{
    display->setDisplayMode(newMode);

    if(bmeReader.isActive())
    {
        display->drawSensorReadings(sensorTemp, sensorHumidity, sensorPressure);
    }
    display->drawCurrentWeather(currentWeatherClient.getCurrentData());
    display->drawForecastWeather(forecastWeatherClient.isValidData(), forecastWeatherClient.getDailyForecasts(), forecastWeatherClient.getForecastCount());
    display->drawCurrentTime(timeClient.getEpochTime());
}

// wifi

void connectWifiCallback()
{
    AsyncWiFiManager wifiManager(webServer.getServer(), &dns);

    display->drawStartupDisplay();

    //wifiManager.resetSettings();
    wifiManager.autoConnect("NospigWeather");

    Serial.println(WiFi.localIP());

    settingsManager.init();

    webServer.init(&settingsManager);
    thingSpeakReporter.init(&settingsManager);
    bmeReader.init(BME_SDA, BME_SCL, BME_ADDRESS);

    delay(WIFI_CONNECTING_DELAY);
    display->setDisplayMode(settingsManager.getDisplayMode());
    display->startMainDisplay();

    // start all tasks
    taskScheduler.addTask(getTime);
    taskScheduler.addTask(getCurrentWeather);
    taskScheduler.addTask(getForecastWeather);
    taskScheduler.addTask(readSensors);
    taskScheduler.addTask(updateThingSpeak);
    taskScheduler.addTask(updateWiFiStrength);
    taskScheduler.addTask(checkSettingsChanged);
    taskScheduler.addTask(checkScreenGrabRequested);

    updateThingSpeak.disable(); 
    getTime.enable();
    getCurrentWeather.enable();
    getForecastWeather.enable();
    readSensors.enable();
    updateWiFiStrength.enable();
    checkSettingsChanged.enable();
    checkScreenGrabRequested.enable();
}

void updateWifiStrengthCallback()
{
    long wifiStrength = WiFi.RSSI();
    display->drawWiFiStrength(wifiStrength);
}

// settings

void checkSettingsChangedCallback()
{
    if(settingsManager.getSettingsChanged())
    {
        //Serial.println("Settings changed.");
        settingsManager.resetSettingsChanged();

        // best just to force a display clear when changing settings
        display->setDisplayMode(settingsManager.getDisplayMode());
        display->restartMainDisplay();

        getTime.forceNextIteration();
        getCurrentWeather.forceNextIteration();
        getForecastWeather.forceNextIteration();
        readSensors.forceNextIteration();
        updateWiFiStrength.forceNextIteration();
    }
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

// basic setup and loop

void setup() 
{
    Serial.begin(115200);

    SPIFFS.begin();

    //display = new DisplaySerial();
    display = new DisplayTFT();

    currentWeatherClient.setMetric(true);
    currentWeatherClient.setLanguage("en");

    forecastWeatherClient.setMetric(true);
    forecastWeatherClient.setLanguage("en");

    taskScheduler.startNow(); 
    taskScheduler.addTask(connectWifi);
    connectWifi.enable();
}

void loop() 
{
    taskScheduler.execute();
}