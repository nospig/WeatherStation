#ifndef _DisplayBase_h
#define _DisplayBase_h

#include "OpenWeatherMapCurrent.h"
#include "OpenWeatherMapDaily.h"

enum DisplayMode
{
    DisplayMode_1,
    DisplayMode_2,
    DisplayMode_3,
};

class DisplayBase
{
    public:
        DisplayBase() {};

        virtual void drawStartupDisplay() {};
        virtual void startMainDisplay() {};
        virtual void restartMainDisplay() {};

        virtual void drawCurrentTime(unsigned long epochTime) {};        
        virtual void drawSensorReadings(float temp, float humidity, float pressure) {};
        virtual void drawCurrentWeather(OpenWeatherMapCurrentData* currentWeather) {};
        virtual void drawForecastWeather(bool validData, OpenWeatherMapDailyData* forecastWeather, int forecastCount) {};
        virtual void drawWiFiStrength(long dBm) {};
        virtual void serveScreenShot() {};

        virtual void setDisplayMode(DisplayMode mode);
        DisplayMode getDisplayMode();

    private:
        DisplayMode displayMode = DisplayMode_1;
};


#endif // _DisplayBase_h
