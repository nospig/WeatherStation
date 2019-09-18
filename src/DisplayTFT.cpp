#include <Arduino.h>
#include "DisplayTFT.h"
#include "SPI.h"
#include "TFT_eSPI.h"
#include <time.h>
#include "icons/weatherIcons.h"

const char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
const char daysOfTheWeekLong[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

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
    tft->setTextDatum(BC_DATUM);
    tft->setTextColor(TFT_WHITE, BACKGROUND_COLOUR); 
    tft->drawString("Connecting.", tft->width()/2, tft->height()/2);
}

void DisplayTFT::startMainDisplay()
{
    tft->fillScreen(BACKGROUND_COLOUR);
    drawStaticElements();
}

void DisplayTFT::restartMainDisplay()
{
    startMainDisplay();
}

void DisplayTFT::setDisplayMode(DisplayMode mode)
{
    DisplayBase::setDisplayMode(mode);

    // do display changing logic, going to assume for now in main display mode
    // caller responsible for updating all elements after making this call
    tft->fillScreen(BACKGROUND_COLOUR);
    drawStaticElements();
}

void DisplayTFT::drawCurrentTime(unsigned long epochTime)
{
    drawTimeDisplay(epochTime, MODE_1_TIME_Y);
}

void DisplayTFT::drawSensorReadings(float temp, float humidity, float pressure)
{
    switch(getDisplayMode())
    {
        case DisplayMode_1:
        case DisplayMode_3:
            drawSensorReadings(temp, humidity, pressure, MODE_1_INDOOR_Y);
            break;
        default:
            break;
    }
}

void DisplayTFT::drawCurrentWeather(OpenWeatherMapCurrentData* currentWeather)
{
    switch(getDisplayMode())
    {
        case DisplayMode_1:
            drawCurrentWeather(currentWeather, MODE_1_CURRENT_Y);
            break;
        case DisplayMode_3:
            drawDetailedCurrentWeather(currentWeather, MODE_3_CURRENT_Y);
        default:
            break;

    }    
}

void DisplayTFT::drawForecastWeather(OpenWeatherMapDailyData* forecastWeather, int forecastCount)
{
    int count;

    switch(getDisplayMode())
    {
        case DisplayMode_1:
            count = min(FORECASTS_MODE_1, forecastCount);
            drawHorizontalForecast(forecastWeather, MODE_1_FORECAST_Y, count);
            break;
        case DisplayMode_2:
            count = min(FORECASTS_MODE_2, forecastCount);
            drawVerticalForecast(forecastWeather, MODE_2_FORECAST_Y, count);
            break;
        default:
            break;
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
 *  Mode 1 only
 * 
 * 
****************************************************************************************/

/****************************************************************************************
 * 
 *  Mode 2 only
 * 
 * 
****************************************************************************************/

void DisplayTFT::drawVerticalForecast(OpenWeatherMapDailyData *forecastWeather, int y, int count)
{
    tft->setTextFont(2);
    tft->setTextDatum(TC_DATUM);
    tft->setTextColor(SECTION_HEADER_COLOUR); 
    tft->drawString("Forecast", tft->width()/2, y+2); 

    y += 24;
    tft->fillRect(0, y, tft->width(), 270, BACKGROUND_COLOUR);

    for(int i=0; i<count; i++)
    {
        drawSingleVerticalForecast(&forecastWeather[i], y);
        y += 52;
    }
}

void DisplayTFT::drawSingleVerticalForecast(OpenWeatherMapDailyData *forecastWeather, int y)
{
    time_t time = forecastWeather->time + forecastWeather->timeZone;
    struct tm* timeInfo;
    timeInfo = gmtime(&time);
    char buffer[32];

    tft->pushImage(2, y, WEATHER_ICON_WIDTH, WEATHER_ICON_HEIGHT, getIconData(forecastWeather->icon));

    tft->setTextFont(2);
    tft->setTextColor(FORECAST_DAY_COLOUR); 
    tft->setTextDatum(TL_DATUM);
    
    int day = (timeInfo->tm_mday-1) % 7;
    sprintf(buffer, "%s - %.0fC", daysOfTheWeekLong[day], forecastWeather->tempMax);
    tft->drawString(buffer, 55, y + 8); 
    tft->drawString(forecastWeather->description, 55, y + tft->fontHeight() + 8); 

}

/****************************************************************************************
 * 
 *  Shared drawing routines
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
    time_t time = forecastWeather->time + forecastWeather->timeZone;
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
    sprintf(buffer, "%.0f | %.0f\n", forecastWeather->tempMin, forecastWeather->tempMax);
    tft->drawString(buffer, x, y); 
}

int DisplayTFT::drawCurrentWeather(OpenWeatherMapCurrentData* currentWeather, int y)
{
    // maybe best just to wipe as not updated often
    tft->fillRect(0, y+20, tft->width(), 60, BACKGROUND_COLOUR);

    if(currentWeather->validData)
    {
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

        return x - widthTemp;
    }

    return 0;
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
    switch(getDisplayMode())
    {
        case DisplayMode_1:
            tft->drawLine(0, MODE_1_CURRENT_Y, tft->width(), MODE_1_CURRENT_Y, SECTION_HEADER_LINE_COLOUR);
            tft->drawLine(0, MODE_1_FORECAST_Y, tft->width(), MODE_1_FORECAST_Y, SECTION_HEADER_LINE_COLOUR); 
            tft->drawLine(0, MODE_1_TIME_Y, tft->width(), MODE_1_TIME_Y, SECTION_HEADER_LINE_COLOUR); 
            break;
        case DisplayMode_2:
            tft->drawLine(0, MODE_1_TIME_Y, tft->width(), MODE_1_TIME_Y, SECTION_HEADER_LINE_COLOUR); 
            break;
        case DisplayMode_3:
            tft->drawLine(0, MODE_1_CURRENT_Y, tft->width(), MODE_1_CURRENT_Y, SECTION_HEADER_LINE_COLOUR);
            tft->drawLine(0, MODE_1_TIME_Y, tft->width(), MODE_1_TIME_Y, SECTION_HEADER_LINE_COLOUR); 
            break;
        default:
            break;
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


/****************************************************************************************
 * 
 *  Mode 3 only
 * 
 * 
****************************************************************************************/

void DisplayTFT::drawDetailedCurrentWeather(OpenWeatherMapCurrentData* currentWeather, int y)
{
    int x;
    char buffer[64];
    time_t time;
    struct tm* timeInfo;

    x = drawCurrentWeather(currentWeather, y);

    y += 85;

    tft->drawLine(0, y, tft->width(), y, SECTION_HEADER_LINE_COLOUR); 

    y += 5;

    tft->fillRect(0, y, tft->width(), 140, BACKGROUND_COLOUR);

    if(currentWeather->validData)
    {
        tft->setTextFont(2);
        tft->setTextColor(CURRENT_WEATHER_CONDITIONS_COLOUR); 
        tft->setTextDatum(TL_DATUM);

        sprintf(buffer, "Min: %.1f | Max: %.1f", currentWeather->tempMin, currentWeather->tempMax);
        tft->drawString(buffer, x, y);    
        y += tft->fontHeight();

        sprintf(buffer, "Humidity: %d%%", currentWeather->humidity);
        tft->drawString(buffer, x, y);    
        y += tft->fontHeight();

        sprintf(buffer, "Pressure: %d hpa", currentWeather->pressure);
        tft->drawString(buffer, x, y);    
        y += tft->fontHeight();

        sprintf(buffer, "Wind: %.1fm/s from %.0f degress", currentWeather->windSpeed, currentWeather->windDeg);
        tft->drawString(buffer, x, y);    
        y += tft->fontHeight();

        if(currentWeather->cloudPercentage > -1)
        {
            sprintf(buffer, "Clouds: %d%%", currentWeather->cloudPercentage);
            tft->drawString(buffer, x, y);    
            y += tft->fontHeight();
        }

        if(currentWeather->rainOneHour > -1)
        {
            sprintf(buffer, "Rain: 1h %dmm, 3h %dmm", currentWeather->rainOneHour, currentWeather->rainThreeHour);
            tft->drawString(buffer, x, y);    
            y += tft->fontHeight();
        }
        
        time = currentWeather->sunRise + currentWeather->timeZone;
        timeInfo = gmtime(&time);
        sprintf(buffer, "Sunrise: %02d:%02d\n", timeInfo->tm_hour, timeInfo->tm_min);
        tft->drawString(buffer, x, y);    
        y += tft->fontHeight();

        time = currentWeather->sunSet + currentWeather->timeZone;
        timeInfo = gmtime(&time);
        sprintf(buffer, "Sunset: %02d:%02d\n", timeInfo->tm_hour, timeInfo->tm_min);
        tft->drawString(buffer, x, y);    
        y += tft->fontHeight();
    }
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