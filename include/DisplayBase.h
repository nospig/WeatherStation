#ifndef _DisplayBase_h
#define _DisplayBase_h

#include "OpenWeatherMapCurrent.h"
#include "OpenWeatherMapDaily.h"
#include "OctoPrintMonitor.h"

enum DisplayMode
{
    DisplayMode_Current,        // current weather
    DisplayMode_Forecast,       // weather forecast
    DisplayMode_Detailed,       // detailed current
    DisplayMode_PrintMonitor,   // print monitor
};

enum ClockFormat
{
    ClockFormat_24h,       
    ClockFormat_AmPm,  
};

enum DateFormat
{
    DateFormat_DDMMYY,       
    DateFormat_MMDDYY,  
};

class DisplayBase
{
    public:
        DisplayBase() {};

        virtual void drawStartupDisplay() {};
        virtual void clearDisplay() {};

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
        void setClockFormat(ClockFormat clockFormat) { this->clockFormat = clockFormat; }
        void setDateFormat(DateFormat dateFormat) { this->dateFormat = dateFormat; }
        ClockFormat getClockFormat() { return clockFormat; }
        DateFormat getDateFormat() { return dateFormat; }

    private:
        DisplayMode displayMode = DisplayMode_Current;
        bool displayMetric = true;
        bool displayEnabled;
        ClockFormat clockFormat;
        DateFormat dateFormat;

};


#endif // _DisplayBase_h
