#include "OpenWeatherMapCurrent.h"
#include "OpenWeatherMapForecast.h"

class DisplayBase
{
    public:
        DisplayBase() {};

        virtual void drawConnectingDisplay() = 0;

        virtual void drawCurrentTime(int day, int hours, int minutes, int seconds) = 0;
        
        virtual void drawSensorReadings(float temp, float humidity) = 0;

        virtual void drawCurrentWeather(OpenWeatherMapCurrentData& currentWeather) = 0;
        virtual void drawForecastWeather(OpenWeatherMapForecastData* forecastWeather, int numForecasts) = 0;
};

