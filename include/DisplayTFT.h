#include "DisplayBase.h"
#include "SPI.h"
#include "TFT_eSPI.h"

class DisplayTFT : public DisplayBase
{
    public:
        DisplayTFT();

        void drawConnectingDisplay();

        void drawCurrentTime(int day, int hours, int minutes, int seconds);

        void drawSensorReadings(float temp, float humidity, float pressure);

        void drawCurrentWeather(OpenWeatherMapCurrentData& currentWeather);
        void drawForecastWeather(OpenWeatherMapForecastData forecastWeather[], int numForecasts);

    private:
        TFT_eSPI *tft;
};