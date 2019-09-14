#include "DisplayBase.h"
#include "SPI.h"
#include "TFT_eSPI.h"

// http://rinkydinkelectronics.com/calc_rgb565.php

#define BACKGROUND_COLOUR                   TFT_BLACK
#define SECTION_HEADER_COLOUR               TFT_YELLOW
#define SECTION_HEADER_LINE_COLOUR          0x75DF
#define SENSOR_READINGS_COLOUR              TFT_YELLOW
#define TIME_TEXT_COLOUR                    0x75DF
#define WIFI_STRENGTH_COLOUR                0x75DF
#define CURRENT_WEATHER_TEMP_COLOUR         TFT_YELLOW
#define CURRENT_WEATHER_CONDITIONS_COLOUR   TFT_YELLOW


class DisplayTFT : public DisplayBase
{
    public:
        DisplayTFT();

        void drawStartupDisplay();
        void startMainDisplay();

        void drawCurrentTime(unsigned long epochTime);

        void drawSensorReadings(float temp, float humidity, float pressure);

        void drawCurrentWeather(OpenWeatherMapCurrentData* currentWeather);
        void drawForecastWeather(OpenWeatherMapDailyData forecastWeather[]);
        void drawWiFiStrength(long dBm);

    private:
        void drawStaticElements();
        void drawCurrentWeather(OpenWeatherMapCurrentData* currentWeather, int y);
        const unsigned short* getIconData(String iconId);
        
        TFT_eSPI *tft;
};