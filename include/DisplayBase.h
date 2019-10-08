#ifndef _DisplayBase_h
#define _DisplayBase_h

#include "OpenWeatherMapCurrent.h"
#include "OpenWeatherMapDaily.h"
#include "OctoPrintMonitor.h"

enum DisplayMode
{
    DisplayMode_1,  // current weather
    DisplayMode_2,  // weather forecast
    DisplayMode_3,  // detailed current
    DisplayMode_4,  // print monitor
};

class DisplayBase
{
    public:
        DisplayBase() {};

        virtual void drawStartupDisplay() {};
        virtual void startMainDisplay() {};
        virtual void restartMainDisplay() {};

        virtual void drawCurrentTime(unsigned long epochTime) {};        
        virtual void drawSensorReadings(float temp, float humidity, float pressure) {};
        virtual void drawCurrentWeather(OpenWeatherMapCurrentData* currentWeather) {};
        virtual void drawForecastWeather(bool validData, OpenWeatherMapDailyData* forecastWeather, int forecastCount) {};
        virtual void drawWiFiStrength(long dBm) {};
        virtual void drawOctoPrintStatus(OctoPrintMonitorData* printData, String printerName, bool enabled) {};
        
        virtual void serveScreenShot() {};
        virtual void setDisplayBrightness(int percent) {};        
        virtual void setDisplayMode(DisplayMode mode);
        DisplayMode getDisplayMode();

        virtual void setDisplayEnabled(bool enabled) { displayEnabled = enabled;};
        bool getDisplayEnabled() { return displayEnabled; }

        void setDisplayMetric(bool metric) { displayMetric = metric; }
        bool getDisplayMetric() { return displayMetric; }
    private:
        DisplayMode displayMode = DisplayMode_1;
        bool displayMetric = true;
        bool displayEnabled;
};


#endif // _DisplayBase_h
