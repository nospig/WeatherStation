#define _TASK_SLEEP_ON_IDLE_RUN

#include <Arduino.h>
#include <TaskScheduler.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <WifiManager.h>
#include "OpenWeatherMapCurrent.h"
#include "OpenWeatherMapForecast.h"
#include "Settings.h"
#include "WeatherStation.h"
#include "Secrets.h"
#include "DisplaySerial.h"
#include "WebServer.h"
#include <FS.h>

// globals
Scheduler taskScheduler;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
WebServer webServer;
DisplayBase* display;
OpenWeatherMapCurrent currentWeatherClient;
OpenWeatherMapCurrentData currentWeather;
OpenWeatherMapForecast forecastWeatherClient;
OpenWeatherMapForecastData forecastWeather[NUM_FORECASTS];

// tasks
Task connectWifi(0, TASK_ONCE, &connectWifiCallback);
Task getTime(TIME_FETCH_INTERVAL, TASK_FOREVER, &getTimeCallback);
Task readSensors(SENSOR_READING_INTERVAL, TASK_FOREVER, &readSensorsCallback);
Task getCurrentWeather(CURRENT_WEATHER_INTERVAL, TASK_FOREVER, &getCurrentWeatherCallback);
Task getForecastWeather(FORECAST_WEATHER_INTERVAL, TASK_FOREVER, &getWeatherForecastCallback);

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
}

// weather

void getCurrentWeatherCallback()
{
    //Serial.println("Get current weather");

    currentWeatherClient.updateCurrentById(&currentWeather, OPEN_WEATHER_MAP_APP_ID, OPEN_WEATHER_MAP_LOCATION_ID);
    display->drawCurrentWeather(&currentWeather);
}

void getWeatherForecastCallback()
{
    int numForecasts;

    //Serial.println("Get forecast weather");

    numForecasts = forecastWeatherClient.updateForecastsById(forecastWeather, OPEN_WEATHER_MAP_APP_ID, OPEN_WEATHER_MAP_LOCATION_ID, NUM_FORECASTS);
    display->drawForecastWeather(forecastWeather, numForecasts);
}

void connectWifiCallback()
{
    WiFiManager wifiManager;

    //wifiManager.resetSettings();
    wifiManager.autoConnect("NospigWeather");

    //Serial.println("Connected");
    Serial.println(WiFi.localIP());

    webServer.init();

    // start all tasks
    taskScheduler.addTask(getTime);
    taskScheduler.addTask(getCurrentWeather);
    taskScheduler.addTask(getForecastWeather);
    taskScheduler.addTask(readSensors);
    
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