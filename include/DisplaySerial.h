#include "DisplayBase.h"

class DisplaySerial : public DisplayBase
{
    public:
        DisplaySerial();

        void drawCurrentTime(unsigned long epochTime);

        void drawSensorReadings(float temp, float humidity, float pressure);

        void drawCurrentWeather(OpenWeatherMapCurrentData* currentWeather);
        void drawForecastWeather(OpenWeatherMapDailyData* forecastWeather, int forecastCount);
};