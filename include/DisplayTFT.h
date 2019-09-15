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
#define FORECAST_DAY_COLOUR                 TFT_YELLOW

// positions for modes

#define MODE_1_INDOOR_Y     0
#define MODE_1_CURRENT_Y    60
#define MODE_1_FORECAST_Y   160
#define MODE_1_TIME_Y       300
#define MODE_1_TIME_HEIGHT  20

#define MODE_2_FORECAST_Y   0

#define WEATHER_ICON_WIDTH 48
#define WEATHER_ICON_HEIGHT 48

#define FORECASTS_MODE_1    3
#define FORECASTS_MODE_2    5

class DisplayTFT : public DisplayBase
{
    public:
        DisplayTFT();

        void drawStartupDisplay();
        void startMainDisplay();

        void drawCurrentTime(unsigned long epochTime);

        void drawSensorReadings(float temp, float humidity, float pressure);

        void drawCurrentWeather(OpenWeatherMapCurrentData* currentWeather);
        void drawForecastWeather(OpenWeatherMapDailyData* forecastWeather, int forecastCount);
        void drawWiFiStrength(long dBm);

        void setDisplayMode(DisplayMode mode);

    private:
        void drawStaticElements();
        void drawCurrentWeather(OpenWeatherMapCurrentData* currentWeather, int y);
        const unsigned short* getIconData(String iconId);
        void drawHorizontalForecast(OpenWeatherMapDailyData *forecastWeather, int y, int count);
        void drawSmallForecast(OpenWeatherMapDailyData *forecastWeather, int y, int x);
        void drawTimeDisplay(unsigned long epochTime, int y);
        void drawSensorReadings(float temp, float humidity, float pressure, int y);

        void drawVerticalForecast(OpenWeatherMapDailyData *forecastWeather, int y, int count);
        void drawSingleVerticalForecast(OpenWeatherMapDailyData *forecastWeather, int y);

        TFT_eSPI *tft;
};