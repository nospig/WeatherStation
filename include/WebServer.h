#ifndef _WebServer_H_
#define _WebServer_H_

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "OpenWeatherMapCurrent.h"
#include "OpenWeatherMapDaily.h"
#include "SettingsManager.h"

class WebServer
{
    public:
        void init(SettingsManager* settingsManager);
        AsyncWebServer* getServer();

        void updateSensorReadings(float temp, float humidity, float pressure);
        void updateCurrentWeather(OpenWeatherMapCurrentData* currentWeather);
        void updateForecastWeather(OpenWeatherMapDailyData* forecastWeather, int forecastCount);

    private:
        static void onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);
        static void updateClientOnConnect();
        static String settingsProcessor(const String& token);
        static void handleUpdateWeatherSettings(AsyncWebServerRequest* request);

        static String currentSensorJson;
        static String currentWeatherJson;
        static String forecastWeatherJson;    

        static SettingsManager* settingsManager;    
};

#endif