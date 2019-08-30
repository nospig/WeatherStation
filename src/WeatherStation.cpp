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

// globals
Scheduler taskScheduler;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

OpenWeatherMapCurrent currentWeatherClient;
OpenWeatherMapCurrentData currentWeather;
OpenWeatherMapForecast forecastWeatherClient;
OpenWeatherMapForecastData forecastWeather[NUM_FORECASTS];

// tasks
Task connectWifi(0, TASK_ONCE, &connectWifiCallback);
Task getTime(TIME_FETCH_INTERVAL, TASK_FOREVER, &getTimeCallback);
Task getCurrentWeather(CURRENT_WEATHER_INTERVAL, TASK_FOREVER, &getCurrentWeatherCallback);
Task getForecastWeather(FORECAST_WEATHER_INTERVAL, TASK_FOREVER, &getWeatherForecastCallback);

// task callbacks

// time

void getTimeCallback()
{
    timeClient.update();
    displayTime();
}

// weather

void getCurrentWeatherCallback()
{
    Serial.println("Get current weather");

    currentWeatherClient.updateCurrentById(&currentWeather, OPEN_WEATHER_MAP_APP_ID, OPEN_WEATHER_MAP_LOCATION_ID);
    displayCurrentWeather();
}

void getWeatherForecastCallback()
{
    Serial.println("Get forecast weather");

    forecastWeatherClient.updateForecastsById(forecastWeather, OPEN_WEATHER_MAP_APP_ID, OPEN_WEATHER_MAP_LOCATION_ID, NUM_FORECASTS);
    displayForecastWeather();
}

void connectWifiCallback()
{
    WiFiManager wifiManager;

    //Serial.println("Connecting to WiFi");
    
    //wifiManager.resetSettings();
    wifiManager.autoConnect("NospigWeather");

    //Serial.println("Connected");
    Serial.println(WiFi.localIP());

    // start all tasks
    taskScheduler.addTask(getTime);
    taskScheduler.addTask(getCurrentWeather);
    taskScheduler.addTask(getForecastWeather);
    
    getTime.enable();
    getCurrentWeather.enable();
    getForecastWeather.enable();
}

// display functions

void displayTime()
{
    Serial.print(daysOfTheWeek[timeClient.getDay()]);
    Serial.print(", ");
    Serial.println(timeClient.getFormattedTime());
}

void displayCurrentWeather()
{
    Serial.println("Current weather");

    Serial.printf("temp %f, humidity %d\n", currentWeather.temp, currentWeather.humidity);
}

void displayForecastWeather()
{
    Serial.println("Forecast weather");

    OpenWeatherMapForecastData* data = &forecastWeather[0];

    time_t time = data->observationTime;
    struct tm* timeInfo;
    timeInfo = gmtime(&time);

    Serial.printf("Day %d, month %d: max temp %f\n", timeInfo->tm_mday, timeInfo->tm_mon, data->tempMax);
}

// basic setup and loop

void setup() 
{
    Serial.begin(115200);

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