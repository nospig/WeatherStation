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

    int y = tft->height();

    tft->setTextDatum(BR_DATUM);
    sprintf(buffer, "%02d:%02d\n", timeInfo->tm_hour, timeInfo->tm_min);
    tft->drawString(buffer, tft->width()/2-30, y); 

    tft->setTextDatum(BC_DATUM);
    int day = (timeInfo->tm_mday-1) % 7;
    tft->drawString(daysOfTheWeek[day], tft->width()/2, y); 

    tft->setTextDatum(BL_DATUM);
    sprintf(buffer, "%d/%d/%02d\n", timeInfo->tm_mday, timeInfo->tm_mon+1, (timeInfo->tm_year+1900) % 100);
    tft->drawString(buffer, tft->width()/2+30, y); 
}

void DisplayTFT::drawSensorReadings(float temp, float humidity, float pressure)
{
    tft->setTextFont(4);
    tft->setTextColor(SENSOR_READINGS_COLOUR, BACKGROUND_COLOUR); 
    tft->setTextPadding(10);

    String tempString = String(temp, 1);
    int center = tft->width()/2;

    tft->setTextDatum(TR_DATUM);
    tft->drawString(tempString + "C", center - 40 , 24);

    tft->setTextDatum(TL_DATUM);
    String humidityString = String(humidity, 0);
    tft->drawString(humidityString + "%", center + 40, 24);
}

void DisplayTFT::drawCurrentWeather(OpenWeatherMapCurrentData* currentWeather)
{
    // TODO, depends on mode?
    drawCurrentWeather(currentWeather, 58);
}

void DisplayTFT::drawCurrentWeather(OpenWeatherMapCurrentData* currentWeather, int y)
{
    // testing, maybe best just to wipe as not updated often
    tft->fillRect(0, y,tft->width(), 75, TFT_LIGHTGREY);

    tft->setTextFont(2);
    tft->setTextDatum(TC_DATUM);
    tft->setTextColor(SECTION_HEADER_COLOUR); 
    tft->drawString(currentWeather->location, tft->width()/2, y); 

    tft->setTextFont(4);
    tft->setTextColor(CURRENT_WEATHER_TEMP_COLOUR); 

    String tempString = String(currentWeather->temp, 1);
    int x = tft->width()/2 - 40;
    int widthTemp;

    tft->setTextDatum(TR_DATUM);
    tft->drawString(tempString + "C", x , y + 24);    
    widthTemp = tft->textWidth(tempString + "C");

    tft->setTextFont(2);
    tft->setTextColor(CURRENT_WEATHER_CONDITIONS_COLOUR); 
    tft->setTextDatum(TL_DATUM);
    tft->drawString(currentWeather->description, x - widthTemp, y + 50);    

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

void DisplayTFT::drawWiFiStrength(long dBm)
{
    int percentage = min(max(2 * ((int)dBm + 100), 0), 100); // how Microsoft convert, linear in the range -100 to -50

    //Serial.printf("wifi: dBm: %ld percent: %d\n", dBm, percentage);

    int x = 4;
    int y = tft->height()-2;
    uint32_t barColour;
    int barValue = 0;
    int barHeight = 2;

    for(int i=0; i<5; i++)
    {
        if(percentage >= barValue)
        {
            barColour = WIFI_STRENGTH_COLOUR;
        }
        else
        {
            barColour = BACKGROUND_COLOUR;
        }
        tft->drawLine(x, y, x, y-barHeight, barColour);
        
        barHeight += 2;
        barValue += 20;
        x += 2;
    }
}

