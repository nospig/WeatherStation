#include <Arduino.h>
#include <TaskScheduler.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESPAsyncWifiManager.h>
#include "OpenWeatherMapCurrent.h"
 #include "OpenWeatherMapDaily.h"
#include "Settings.h"
#include "WeatherStation.h"
#include "Secrets.h"
#include "DisplaySerial.h"
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
    thingSpeakReporter.sendSensorReadings(sensorTemp, sensorHumidity, sensorPressure);
}

// weather

void getCurrentWeatherCallback()
{
    //Serial.println("Get current weather");

    currentWeatherClient.updateById(OPEN_WEATHER_MAP_APP_ID, OPEN_WEATHER_MAP_LOCATION_ID);
    display->drawCurrentWeather(currentWeatherClient.getCurrentData());
    webServer.updateCurrentWeather(currentWeatherClient.getCurrentData());

    WiFi.RSSI();
}

void getWeatherForecastCallback()
{
    //Serial.println("Get forecast weather");

    forecastWeatherClient.updateById(OPEN_WEATHER_MAP_APP_ID, OPEN_WEATHER_MAP_LOCATION_ID);
    display->drawForecastWeather(forecastWeatherClient.getDailyForecasts(), forecastWeatherClient.getForecastCount());
    webServer.updateForecastWeather(forecastWeatherClient.getDailyForecasts(), forecastWeatherClient.getForecastCount());
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
    display->drawForecastWeather(forecastWeatherClient.getDailyForecasts(), forecastWeatherClient.getForecastCount());
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

    webServer.init();
    thingSpeakReporter.init();
    bmeReader.init(BME_SDA, BME_SCL, BME_ADDRESS);

    delay(WIFI_CONNECTING_DELAY);
    display->startMainDisplay();

    // start all tasks
    taskScheduler.addTask(getTime);
    taskScheduler.addTask(getCurrentWeather);
    taskScheduler.addTask(getForecastWeather);
    taskScheduler.addTask(readSensors);
    taskScheduler.addTask(updateThingSpeak);
    taskScheduler.addTask(updateWiFiStrength);
    updateThingSpeak.disable();
    
    getTime.enable();
    getCurrentWeather.enable();
    getForecastWeather.enable();
    readSensors.enable();
    updateWiFiStrength.enable();
}

void updateWifiStrengthCallback()
{
    long wifiStrength = WiFi.RSSI();
    display->drawWiFiStrength(wifiStrength);
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