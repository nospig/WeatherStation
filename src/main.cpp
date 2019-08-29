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

// globals
Scheduler taskScheduler;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

// task callbacks
void connectWifiCallback();
void getTimeCallback();
void getCurrentWeatherCallback();
void getWeatherForecastCallback();

// task prototypes
Task connectWifi(0, TASK_ONCE, &connectWifiCallback);
Task getTime(TIME_FETCH_INTERVAL, TASK_FOREVER, &getTimeCallback);

// task callbacks

void connectWifiCallback()
{
    WiFiManager wifiManager;

    Serial.println("Connecting to WiFi");
    
    //wifiManager.resetSettings();
    wifiManager.autoConnect("NospigWeather");

    Serial.println("Connected");
    Serial.println(WiFi.localIP());

    // start all tasks
    taskScheduler.addTask(getTime);
    getTime.enable();
}

void getTimeCallback()
{
    timeClient.update();

    Serial.print(daysOfTheWeek[timeClient.getDay()]);
    Serial.print(", ");
    Serial.println(timeClient.getFormattedTime());
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