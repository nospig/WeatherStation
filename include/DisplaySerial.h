#include "DisplayBase.h"

class DisplaySerial : public DisplayBase
{
    public:
        DisplaySerial();

        void drawCurrentTime(int day, int hours, int minutes, int seconds);

        void drawSensorReadings(float temp, float humidity, float pressure);

        void drawCurrentWeather(OpenWeatherMapCurrentData& currentWeather);
        void drawForecastWeather(OpenWeatherMapDailyData forecastWeather[]);
};