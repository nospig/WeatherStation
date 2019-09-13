#include <Arduino.h>
#include "DisplayTFT.h"
#include "SPI.h"
#include "TFT_eSPI.h"

const char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

DisplayTFT::DisplayTFT()
{
    tft = new TFT_eSPI();

    tft->begin();
    tft->setRotation(0);

    tft->fillRect(0, 0, 240, 320, TFT_RED);
}

void DisplayTFT::drawConnectingDisplay()
{

}

void DisplayTFT::drawCurrentTime(int day, int hours, int minutes, int seconds)
{
    Serial.print(daysOfTheWeek[day]);
    Serial.print(", ");
    Serial.printf("%02d:%02d:%02d\n", hours, minutes, seconds);
}

void DisplayTFT::drawSensorReadings(float temp, float humidity, float pressure)
{

}

void DisplayTFT::drawCurrentWeather(OpenWeatherMapCurrentData& currentWeather)
{
    Serial.println("Current weather");

    Serial.printf("temp %f, humidity %d\n", currentWeather.temp, currentWeather.humidity);

}

void DisplayTFT::drawForecastWeather(OpenWeatherMapForecastData* forecastWeather, int numForecasts)
{
    Serial.printf("Forecast weather for %d forecasts\n", numForecasts);

    OpenWeatherMapForecastData* data = &forecastWeather[0];

    time_t time = data->observationTime;
    struct tm* timeInfo;
    timeInfo = gmtime(&time);

    Serial.printf("Day %d, month %d: max temp %f\n", timeInfo->tm_mday, timeInfo->tm_mon, data->tempMax);
}