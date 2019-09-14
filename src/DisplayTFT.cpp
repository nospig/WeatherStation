#include <Arduino.h>
#include "DisplayTFT.h"
#include "SPI.h"
#include "TFT_eSPI.h"
#include <time.h>

const char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

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
    tft->setTextFont(2);
    tft->setTextDatum(TC_DATUM);
    tft->setTextColor(SECTION_HEADER_COLOUR, BACKGROUND_COLOUR); // Set the font colour AND the background colour
    tft->drawString("Indoor", tft->width()/2, 2); 

    tft->drawLine(0, 55, tft->width(), 55, SECTION_HEADER_LINE_COLOUR); // below indoor readings
    tft->drawLine(0, tft->height()-20, tft->width(), tft->height()-20, SECTION_HEADER_LINE_COLOUR); // above time section
}

void DisplayTFT::drawCurrentTime(unsigned long epochTime)
{
    tft->setTextFont(2);
    tft->setTextPadding(10);
    tft->setTextColor(TIME_TEXT_COLOUR, BACKGROUND_COLOUR); // Set the font colour AND the background colour
   
    time_t time = epochTime;
    struct tm* timeInfo;
    timeInfo = gmtime(&time);
    char buffer[16];

    tft->setTextDatum(BR_DATUM);
    sprintf(buffer, "%02d:%02d\n", timeInfo->tm_hour, timeInfo->tm_min);
    tft->drawString(buffer, tft->width()/2-30, tft->height()-2); 

    tft->setTextDatum(BC_DATUM);
    int day = (timeInfo->tm_mday-1) % 7;
    tft->drawString(daysOfTheWeek[day], tft->width()/2, tft->height()-2); 

    tft->setTextDatum(BL_DATUM);
    sprintf(buffer, "%d/%d/%02d\n", timeInfo->tm_mday, timeInfo->tm_mon+1, (timeInfo->tm_year+1900) % 100);
    tft->drawString(buffer, tft->width()/2+30, tft->height()-2); 
}

void DisplayTFT::drawSensorReadings(float temp, float humidity, float pressure)
{
    tft->setTextFont(4);
    tft->setTextDatum(TL_DATUM);
    tft->setTextColor(SENSOR_READINGS_COLOUR, BACKGROUND_COLOUR); 
    tft->setTextPadding(10);

    String tempString = String(temp, 1);
    tft->drawString(tempString + "C", 22, 24);

    tft->setTextDatum(TR_DATUM);
    String humidityString = String(humidity, 0);
    tft->drawString(humidityString + "%", 240-22, 24);
}

void DisplayTFT::drawCurrentWeather(OpenWeatherMapCurrentData* currentWeather)
{
    Serial.println("Current weather");

    Serial.printf("temp %f, humidity %d\n", currentWeather->temp, currentWeather->humidity);

}

void DisplayTFT::drawForecastWeather(OpenWeatherMapDailyData* forecastWeather)
{
    //Serial.printf("Forecast weather for %d forecasts\n", numForecasts);

/*
    OpenWeatherMapForecastData* data = &forecastWeather[0];

    time_t time = data->observationTime;
    struct tm* timeInfo;
    timeInfo = gmtime(&time);

    Serial.printf("Day %d, month %d: max temp %f\n", timeInfo->tm_mday, timeInfo->tm_mon, data->tempMax);
    */
}

