#ifndef _DisplayBase_h
#define _DisplayBase_h

#include "OpenWeatherMapCurrent.h"
#include "OpenWeatherMapDaily.h"

enum CurrentDisplayMode
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

        virtual void drawCurrentTime(int day, int hours, int minutes, int seconds);        
        virtual void drawSensorReadings(float temp, float humidity, float pressure);
        virtual void drawCurrentWeather(OpenWeatherMapCurrentData& currentWeather);
        virtual void drawForecastWeather(OpenWeatherMapDailyData* forecastWeather);

        void setDisplayMode(CurrentDisplayMode mode);
        CurrentDisplayMode getDisplayMode();

    private:
        CurrentDisplayMode displayMode = DisplayMode_1;
};


#endif // _DisplayBase_h
