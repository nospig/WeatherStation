#include <Arduino.h>
#include "DisplayTFT.h"
#include "SPI.h"
#include "TFT_eSPI.h"
#include <time.h>
#include "icons/weatherIcons.h"

const char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

DisplayTFT::DisplayTFT()
{
    tft = new TFT_eSPI();

    tft->begin();
    tft->setRotation(2);

    // Swap the colour byte order when rendering for images
    tft->setSwapBytes(true);
}
 
void DisplayTFT::drawStartupDisplay()
{
    tft->fillScreen(BACKGROUND_COLOUR);

    tft->setTextFont(4);
    tft->setTextDatum(MC_DATUM);
    tft->setTextColor(TFT_WHITE, BACKGROUND_COLOUR); 
    tft->drawString("Connecting.", tft->width()/2, tft->height()/2);
}

void DisplayTFT::startMainDisplay()
{
    tft->fillScreen(BACKGROUND_COLOUR);
    drawStaticElements();
}

void DisplayTFT::drawCurrentTime(unsigned long epochTime)
{
    drawTimeDisplay(epochTime, MODE_1_TIME_Y);
}

void DisplayTFT::drawSensorReadings(float temp, float humidity, float pressure)
{
    drawSensorReadings(temp, humidity, pressure, MODE_1_INDOOR_Y);
}

void DisplayTFT::drawCurrentWeather(OpenWeatherMapCurrentData* currentWeather)
{
    // TODO, depends on mode?
    drawCurrentWeather(currentWeather, MODE_1_CURRENT_Y);
}

void DisplayTFT::drawForecastWeather(OpenWeatherMapDailyData* forecastWeather, int forecastCount)
{
    // TODO, depending on screen mode
    if(forecastCount >= 3)
    {
        drawHorizontalForecast(forecastWeather, MODE_1_FORECAST_Y, 3);
    }
}

void DisplayTFT::drawWiFiStrength(long dBm)
{
    int percentage = min(max(2 * ((int)dBm + 100), 0), 100); // how Microsoft convert, linear in the range -100 to -50

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

/****************************************************************************************
 * 
 * 
 * 
 * 
****************************************************************************************/

void DisplayTFT::drawHorizontalForecast(OpenWeatherMapDailyData *forecastWeather, int y, int count)
{
    tft->setTextFont(2);
    tft->setTextDatum(TC_DATUM);
    tft->setTextColor(SECTION_HEADER_COLOUR); 
    tft->drawString("Forecast", tft->width()/2, y+2); 

    // maybe best just to wipe as not updated often
    tft->fillRect(0, y+30, tft->width(), 90, BACKGROUND_COLOUR);

    int width = tft->width() / (count+1);
    int x = width;

    for(int i=0; i<count; i++)
    {
        drawSmallForecast(&forecastWeather[i], y+30, x);
        x += width;
    }
}

void DisplayTFT::drawSmallForecast(OpenWeatherMapDailyData *forecastWeather, int y, int x)
{
    time_t time = forecastWeather->time;
    struct tm* timeInfo;
    timeInfo = gmtime(&time);
    char buffer[16];

    tft->setTextFont(2);
    tft->setTextColor(FORECAST_DAY_COLOUR); 
    tft->setTextDatum(TC_DATUM);
    int day = (timeInfo->tm_mday-1) % 7;
    tft->drawString(daysOfTheWeek[day], x, y); 

    y += tft->fontHeight();

    tft->pushImage(x - (WEATHER_ICON_WIDTH/2), y, WEATHER_ICON_WIDTH, WEATHER_ICON_HEIGHT, getIconData(forecastWeather->icon));

    y += WEATHER_ICON_HEIGHT + 4;

    tft->setTextDatum(TC_DATUM);
    sprintf(buffer, "%.0f|%.0f\n", forecastWeather->tempMin, forecastWeather->tempMax);
    tft->drawString(buffer, x, y); 
}

void DisplayTFT::drawCurrentWeather(OpenWeatherMapCurrentData* currentWeather, int y)
{
    // maybe best just to wipe as not updated often
    tft->fillRect(0, y+20, tft->width(), 60, BACKGROUND_COLOUR);

    tft->setTextFont(2);
    tft->setTextDatum(TC_DATUM);
    tft->setTextColor(SECTION_HEADER_COLOUR); 
    tft->drawString(currentWeather->location, tft->width()/2, y+2); 

    tft->setTextFont(4);
    tft->setTextColor(CURRENT_WEATHER_TEMP_COLOUR); 

    String tempString = String(currentWeather->temp, 1);
    int x = tft->width()/2 - 40;
    int widthTemp;

    tft->setTextDatum(TR_DATUM);
    tft->drawString(tempString + "C", x , y + 32);    
    widthTemp = tft->textWidth(tempString + "C");

    tft->setTextFont(2);
    tft->setTextColor(CURRENT_WEATHER_CONDITIONS_COLOUR); 
    tft->setTextDatum(TL_DATUM);
    tft->drawString(currentWeather->description, x - widthTemp, y + 58);    

    tft->pushImage(160, y+30, WEATHER_ICON_WIDTH, WEATHER_ICON_HEIGHT, getIconData(currentWeather->icon));
}

void DisplayTFT::drawSensorReadings(float temp, float humidity, float pressure, int y)
{
    tft->setTextFont(2);
    tft->setTextDatum(TC_DATUM);
    tft->setTextColor(SECTION_HEADER_COLOUR, BACKGROUND_COLOUR);
    tft->drawString("Indoor", tft->width()/2, y+2); 

    tft->setTextFont(4);
    tft->setTextColor(SENSOR_READINGS_COLOUR, BACKGROUND_COLOUR); 
    tft->setTextPadding(10);

    String tempString = String(temp, 1);
    int center = tft->width()/2;

    tft->setTextDatum(TR_DATUM);
    tft->drawString(tempString + "C", center - 40 , y+26);

    tft->setTextDatum(TL_DATUM);
    String humidityString = String(humidity, 0);
    tft->drawString(humidityString + "%", center + 40, y+26);
}

void DisplayTFT::drawStaticElements()
{
    if(getDisplayMode() == DisplayMode_1)
    {
        tft->drawLine(0, MODE_1_CURRENT_Y, tft->width(), MODE_1_CURRENT_Y, SECTION_HEADER_LINE_COLOUR);
        tft->drawLine(0, MODE_1_FORECAST_Y, tft->width(), MODE_1_FORECAST_Y, SECTION_HEADER_LINE_COLOUR); 
        tft->drawLine(0, MODE_1_TIME_Y, tft->width(), MODE_1_TIME_Y, SECTION_HEADER_LINE_COLOUR); 
    }
}

void DisplayTFT::drawTimeDisplay(unsigned long epochTime, int y)
{
    tft->setTextFont(2);
    tft->setTextPadding(10);
    tft->setTextColor(TIME_TEXT_COLOUR, BACKGROUND_COLOUR); // Set the font colour AND the background colour
   
    time_t time = epochTime;
    struct tm* timeInfo;
    timeInfo = gmtime(&time);
    char buffer[16];

    y += MODE_1_TIME_HEIGHT;

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

const unsigned short* DisplayTFT::getIconData(String iconId)
{    
    // convert icon code from weather to our image, not all handled
    if(iconId == "01d")
    {
        return icon_01d;
    }
    if(iconId == "02d")
    {
        return icon_02d;
    }
    if(iconId == "03d")
    {
        return icon_03d;
    }
    if(iconId == "04d")
    {
        return icon_03d;
    }
    if(iconId == "09d")
    {
        return icon_09d;
    }
    if(iconId == "10d")
    {
        return icon_10d;
    }
    if(iconId == "11d")
    {
        return icon_11d;
    }
    if(iconId == "13d")
    {
        return icon_13d;
    }
    if(iconId == "50d")
    {
        return icon_50d;
    }
    if(iconId == "01n")
    {
        return icon_01n;
    }
    if(iconId == "02n")
    {
        return icon_02n;
    }
    if(iconId == "03n")
    {
        return icon_03d;
    }
    if(iconId == "04n")
    {
        return icon_03d;
    }
    if(iconId == "09n")
    {
        return icon_09n;
    }
    if(iconId == "10n")
    {
        return icon_09n;
    }
    if(iconId == "11n")
    {
        return icon_11n;
    }
    if(iconId == "13n")
    {
        return icon_13n;
    }
    if(iconId == "50n")
    {
        return icon_50d;
    }

    Serial.println("Icon: " + iconId + " not handled.");

    return icon_01d;
}