#define _TASK_SLEEP_ON_IDLE_RUN

#include <Arduino.h>
#include <TaskScheduler.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESPAsyncWifiManager.h>
#include "OpenWeatherMapCurrent.h"
#include "OpenWeatherMapForecast.h"
#include "Settings.h"
#include "WeatherStation.h"
#include "Secrets.h"
#include "DisplaySerial.h"
#include "WebServer.h"
#include "ThingSpeakReporter.h"
#include <FS.h>

// globals
Scheduler taskScheduler;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
WebServer webServer;
ThingSpeakReporter thingSpeakReporter;
DisplayBase* display;
OpenWeatherMapCurrent currentWeatherClient;
OpenWeatherMapCurrentData currentWeather;
OpenWeatherMapForecast forecastWeatherClient;
OpenWeatherMapForecastData forecastWeather[NUM_FORECASTS];
DNSServer dns;
float sensorTemp, sensorHumidity, sensorPressure;

// tasks
Task connectWifi(0, TASK_ONCE, &connectWifiCallback);
Task getTime(TIME_FETCH_INTERVAL, TASK_FOREVER, &getTimeCallback);
Task readSensors(SENSOR_READING_INTERVAL, TASK_FOREVER, &readSensorsCallback);
Task getCurrentWeather(CURRENT_WEATHER_INTERVAL, TASK_FOREVER, &getCurrentWeatherCallback);
Task getForecastWeather(FORECAST_WEATHER_INTERVAL, TASK_FOREVER, &getWeatherForecastCallback);
Task updateThingSpeak(THINGSPEAK_REPORTING_INTERVAL, TASK_FOREVER, &updateThingSpeakCallback);

// task callbacks

// time

void getTimeCallback()
{
    timeClient.update();
    display->drawCurrentTime(timeClient.getDay(), timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds());
}

// sensors
void readSensorsCallback()
{
    Serial.println("Read sensors now");

    // testing data
    sensorTemp = 32.3;
    sensorHumidity = 88;
    sensorPressure = 1010;

    display->drawSensorReadings(sensorTemp, sensorHumidity, sensorPressure);
    webServer.updateSensorReadings(sensorTemp, sensorHumidity, sensorPressure);

    // only enable thing speak after some data recorded
    updateThingSpeak.enableIfNot();
}

// ThingSpeak

void updateThingSpeakCallback()
{
    // just send the latest sensor saved readings, no need to update again, avoid chances of updating the sensor too soon
    // testing thingspeak
    thingSpeakReporter.sendSensorReadings(sensorTemp, sensorHumidity, sensorPressure);
}

// weather

void getCurrentWeatherCallback()
{
    //Serial.println("Get current weather");

    currentWeatherClient.updateCurrentById(&currentWeather, OPEN_WEATHER_MAP_APP_ID, OPEN_WEATHER_MAP_LOCATION_ID);
    display->drawCurrentWeather(currentWeather);
    webServer.updateCurrentWeather(currentWeather);
}

void getWeatherForecastCallback()
{
    int numForecasts;

    //Serial.println("Get forecast weather");

    numForecasts = forecastWeatherClient.updateForecastsById(forecastWeather, OPEN_WEATHER_MAP_APP_ID, OPEN_WEATHER_MAP_LOCATION_ID, NUM_FORECASTS);
    display->drawForecastWeather(forecastWeather, numForecasts);
    webServer.updateForecastWeather(forecastWeather, numForecasts);
}

void connectWifiCallback()
{
    AsyncWiFiManager wifiManager(webServer.getServer(), &dns);

    //wifiManager.resetSettings();
    wifiManager.autoConnect("NospigWeather");

    Serial.println(WiFi.localIP());

    webServer.init();
    thingSpeakReporter.init();

    // start all tasks
    taskScheduler.addTask(getTime);
    taskScheduler.addTask(getCurrentWeather);
    taskScheduler.addTask(getForecastWeather);
    taskScheduler.addTask(readSensors);
    taskScheduler.addTask(updateThingSpeak);
    updateThingSpeak.disable();
    
    getTime.enable();
    getCurrentWeather.enable();
    getForecastWeather.enable();
    readSensors.enable();
}

// basic setup and loop

void setup() 
{
    Serial.begin(115200);

    SPIFFS.begin();

    display = new DisplaySerial();

    currentWeatherClient.setMetric(true);
    currentWeatherClient.setLanguage("en");

    forecastWeatherClient.setMetric(true);
    forecastWeatherClient.setLanguage("en");
    forecastWeatherClient.setAllowedHours(forecastHours, sizeof(forecastHours));

    taskScheduler.startNow(); 
    taskScheduler.addTask(connectWifi);
    connectWifi.enable();
}

void loop() 
{
    taskScheduler.execute();
}