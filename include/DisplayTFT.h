#include "DisplayBase.h"
#include "SPI.h"
#include "TFT_eSPI.h"

#define BACKGROUND_COLOUR TFT_BLACK
#define SECTION_HEADER_COLOUR TFT_YELLOW

class DisplayTFT : public DisplayBase
{
    public:
        DisplayTFT();

        void drawStartupDisplay();
        void startMainDisplay();

        void drawCurrentTime(int day, int hours, int minutes, int seconds);

        void drawSensorReadings(float temp, float humidity, float pressure);

        void drawCurrentWeather(OpenWeatherMapCurrentData* currentWeather);
        void drawForecastWeather(OpenWeatherMapDailyData forecastWeather[]);

    private:
        void drawStaticElements();

        TFT_eSPI *tft;
};