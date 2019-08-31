#include <Arduino.h>
#include "DisplayLCD.h"

const char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

DisplayLCD::DisplayLCD()
{

}

void DisplayLCD::drawConnectingDisplay()
{

}

void DisplayLCD::drawCurrentTime(int day, int hours, int minutes, int seconds)
{
    Serial.print(daysOfTheWeek[day]);
    Serial.print(", ");
    Serial.printf("%02d:%02d:%02d\n", hours, minutes, seconds);
}

void DisplayLCD::drawSensorReadings(float temp, float humidity)
{

}

void DisplayLCD::drawCurrentWeather(OpenWeatherMapCurrentData& currentWeather)
{
    Serial.println("Current weather");

    Serial.printf("temp %f, humidity %d\n", currentWeather.temp, currentWeather.humidity);

}

void DisplayLCD::drawForecastWeather(OpenWeatherMapForecastData* forecastWeather, int numForecasts)
{
    Serial.printf("Forecast weather for %d forecasts\n", numForecasts);

    OpenWeatherMapForecastData* data = &forecastWeather[0];

    time_t time = data->observationTime;
    struct tm* timeInfo;
    timeInfo = gmtime(&time);

    Serial.printf("Day %d, month %d: max temp %f\n", timeInfo->tm_mday, timeInfo->tm_mon, data->tempMax);
}