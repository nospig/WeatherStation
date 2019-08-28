#define _TASK_SLEEP_ON_IDLE_RUN

#include <Arduino.h>
#include <TaskScheduler.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <WifiManager.h>
#include "OpenWeatherMapCurrent.h"
#include "OpenWeatherMapForecast.h"

// globals
Scheduler taskScheduler;

// task callbacks
void connectWifiCallback();
void getTimeCallback();
void getCurrentWeatherCallback();
void getWeatherForecastCallback();

// task prototypes
Task connectWifi(0, TASK_ONCE, &connectWifiCallback);


// tasks

void connectWifiCallback()
{
    WiFiManager wifiManager;

    Serial.println("Connecting to WiFi");
    
    //wifiManager.resetSettings();
    wifiManager.autoConnect("NospigWeather");

    Serial.println("Connected");
    Serial.println(WiFi.localIP());

    //runner.addTask(getTime);
    //getTime.enable();
}


// basic setup and loop

void setup() 
{
    Serial.begin(115200);

    taskScheduler.startNow(); 
    taskScheduler.addTask(connectWifi);
    connectWifi.enable();
}

void loop() 
{
    taskScheduler.execute();
}