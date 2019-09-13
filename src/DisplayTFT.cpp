#include <Arduino.h>
#include "DisplayTFT.h"
#include "SPI.h"
#include "TFT_eSPI.h"
#include <FS.h>

const char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

DisplayTFT::DisplayTFT()
{
    tft = new TFT_eSPI();

    tft->begin();
    tft->setRotation(2);
}
 
void DisplayTFT::drawStartupDisplay()
{
    tft->fillScreen(BACKGROUND_COLOUR);

    tft->setTextFont(4);
    tft->setTextDatum(MC_DATUM);
    tft->setTextColor(TFT_WHITE, BACKGROUND_COLOUR); 
    tft->drawString("Connecting.", tft->width()/2, tft->height()/2);
    tft->unloadFont();
}

void DisplayTFT::startMainDisplay()
{
    tft->fillScreen(BACKGROUND_COLOUR);
    drawStaticElements();
}

void DisplayTFT::drawStaticElements()
{
    // testing

    tft->setTextFont(2);
    tft->setTextDatum(TC_DATUM);
    tft->setTextColor(SECTION_HEADER_COLOUR, BACKGROUND_COLOUR); // Set the font colour AND the background colour
    tft->drawString("Indoor", tft->width()/2, 2); 
}

void DisplayTFT::drawCurrentTime(int day, int hours, int minutes, int seconds)
{
    // testing

    tft->setTextFont(4);
    tft->setTextDatum(TL_DATUM);
    tft->setTextColor(SECTION_HEADER_COLOUR, BACKGROUND_COLOUR); // Set the font colour AND the background colour
    tft->drawString("32C", 22, 24);

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

