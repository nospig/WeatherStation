#include <Arduino.h>
#include "DisplaySerial.h"

const char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

DisplaySerial::DisplaySerial()
{

}

void DisplaySerial::drawCurrentTime(int day, int hours, int minutes, int seconds)
{
    Serial.print(daysOfTheWeek[day]);
    Serial.print(", ");
    Serial.printf("%02d:%02d:%02d\n", hours, minutes, seconds);
}

void DisplaySerial::drawSensorReadings(float temp, float humidity, float pressure)
{
    Serial.printf("Sensor readings: Temp:%f, humidity:%f, pressure:%f\n", temp, humidity, pressure);
}

void DisplaySerial::drawCurrentWeather(OpenWeatherMapCurrentData& currentWeather)
{
    Serial.println("Current weather");

    Serial.printf("temp %f, humidity %d\n", currentWeather.temp, currentWeather.humidity);

}

void DisplaySerial::drawForecastWeather(OpenWeatherMapForecastData* forecastWeather, int numForecasts)
{
    Serial.printf("Forecast weather for %d forecasts\n", numForecasts);

    OpenWeatherMapForecastData* data = &forecastWeather[0];

    time_t time = data->observationTime;
    struct tm* timeInfo;
    timeInfo = gmtime(&time);

    Serial.printf("Day %d, month %d: max temp %f\n", timeInfo->tm_mday, timeInfo->tm_mon, data->tempMax);
}