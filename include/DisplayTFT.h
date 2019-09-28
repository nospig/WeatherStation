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

#define MODE_3_CURRENT_Y    MODE_1_CURRENT_Y

#define WEATHER_ICON_WIDTH  48
#define WEATHER_ICON_HEIGHT 48

#define FORECASTS_MODE_1    3
#define FORECASTS_MODE_2    5

#define PIXEL_TIMEOUT 100     // 100ms Time-out between pixel requests
#define START_TIMEOUT 10000   // 10s Maximum time to wait at start transfer

#define BITS_PER_PIXEL 16     // 24 for RGB colour format, 16 for 565 colour format

// File names must be alpha-numeric characters (0-9, a-z, A-Z) or "/" underscore "_"
// other ascii characters are stripped out by client, including / generates
// sub-directories
#define DEFAULT_FILENAME "tft_screenshots/screenshot" // In case none is specified
#define FILE_TYPE "png"       // jpg, bmp, png, tif are valid


// Filename extension
// '#' = add 0-9, '@' = add timestamp, '%' add millis() timestamp, '*' = add nothing
// '@' and '%' will generate new unique filenames, so beware of cluttering up your
// hard drive with lots of images! The PC client sketch is set to limit the number of
// saved images to 1000 and will then prompt for a restart.
#define FILE_EXT  '%'         

// Number of pixels to send in a burst (minimum of 1), no benefit above 8
// NPIXELS values and render times: 1 = 5.0s, 2 = 1.75s, 4 = 1.68s, 8 = 1.67s
#define NPIXELS 8  // Must be integer division of both TFT width and TFT height

#define BRIGHTNESS_PIN          16   // D0
#define MAX_BRIGHTNESS_VALUE    1023 // max value of analog out on ESP8266

class DisplayTFT : public DisplayBase
{
    public:
        DisplayTFT();

        void drawStartupDisplay();
        void startMainDisplay();
        void restartMainDisplay();

        void drawCurrentTime(unsigned long epochTime);

        void drawSensorReadings(float temp, float humidity, float pressure);

        void drawCurrentWeather(OpenWeatherMapCurrentData* currentWeather);
        void drawForecastWeather(bool validData, OpenWeatherMapDailyData* forecastWeather, int forecastCount);
        void drawWiFiStrength(long dBm);

        void setDisplayMode(DisplayMode mode);
        void setDisplayEnabled(bool enabled);
        void serveScreenShot();
        void setDisplayBrightness(int percent);
        
    private:
        void drawStaticElements();
        int drawCurrentWeather(OpenWeatherMapCurrentData* currentWeather, int y);
        void drawDetailedCurrentWeather(OpenWeatherMapCurrentData* currentWeather, int y);
        const unsigned short* getIconData(String iconId);
        void drawHorizontalForecast(bool validData, OpenWeatherMapDailyData *forecastWeather, int y, int count);
        void drawSmallForecast(OpenWeatherMapDailyData *forecastWeather, int y, int x);
        void drawTimeDisplay(unsigned long epochTime, int y);
        void drawSensorReadings(float temp, float humidity, float pressure, int y);

        void drawVerticalForecast(bool validData, OpenWeatherMapDailyData *forecastWeather, int y, int count);
        void drawSingleVerticalForecast(OpenWeatherMapDailyData *forecastWeather, int y);

        boolean screenServer(void);
        boolean screenServer(String filename);
        boolean serialScreenServer(String filename);
        void sendParameters(String filename);

        TFT_eSPI *tft;
        int brightness;
};