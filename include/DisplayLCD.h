#include "DisplayBase.h"

class DisplayLCD : public DisplayBase
{
    public:
        DisplayLCD();

        void drawConnectingDisplay();

        void drawCurrentTime(int day, int hours, int minutes, int seconds);

        void drawSensorReadings(float temp, float humidity);

        void drawCurrentWeather(OpenWeatherMapCurrentData& currentWeather);
        void drawForecastWeather(OpenWeatherMapForecastData forecastWeather[], int numForecasts);
};