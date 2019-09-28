#ifndef _settingsmanager_h
#define _settingsmanager_h

#include <ArduinoJson.h>
#include "DisplayBase.h"
#include "Settings.h"

const String SETTINGS_FILE_NAME = "Settings.json";

typedef struct SettingsData
{
    int thingSpeakChannelID;
    String thingSpeakAPIKey;
    
    String openWeatherMapAPIKey;
    String openWeatherLocationID;

    DisplayMode dispayMode;

    int currentWeatherInterval;
    int forecastWeatherInterval;
    int sensorReadingInterval;
    int thingSpeakReportingInterval;

    String mqttBroker;
    String mqttUsername;
    String mqttPassword;
    String mqttTopic;
    int mqttPort;

    bool thingSpeakEnabled;
    bool mqttEnabled;

    long utcOffsetSeconds;

} SettingsData;

class SettingsManager
{
    public:
        void init();

        void resetSettings();

        String getThingSpeakApiKey();
        void setThingSpeakApiKey(String apiKey);

        int getThingSpeakChannelID();
        void setThingSpeakChannelID(int channelID);

        String getOpenWeatherApiKey();
        void setOpenWeatherApiKey(String apiKey);

        String getOpenWeatherlocationID();
        void setOpenWeatherlocationID(String locationID);

        DisplayMode getDisplayMode();
        void setDisplayMode(DisplayMode displayMode);

        int getCurrentWeatherInterval();
        void setCurrentWeatherInterval(int interval);

        int getForecastWeatherInterval();
        void setForecastWeatherInterval(int interval);

        int getSensorReadingInterval();
        void setSensorReadingInterval(int interval);

        int getThingSpeakReportingInterval();
        void setThingSpeakReportingInterval(int interval);

        bool getThingSpeakEnabled();
        void setThingSpeakEnabled(bool enabled);

        bool getMQTTEnabled();
        void setMQTTEnabled(bool enabled);

        int getMQTTPort();
        void setMQTTPort(int port);

        String getMQTTBroker();
        void setMQTTBroker(String url);

        String getMQTTUsername();
        void setMQTTUsername(String userName);

        String getMQTTPassword();
        void setMQTTPassword(String password);

        String getMQTTTopic();
        void setMQTTTopic(String topic);

        bool getMQTTReconnectRequired();
        void resetMQTTReconnectRequired();

        long getUtcOffset();
        void setUtcOffset(long utcOffset);

        bool getSettingsChanged();
        void resetSettingsChanged();

    private:
        SettingsData data;
        bool settingsChanged;
        bool mqttReconnectRequired;

        void loadSettings();
        void saveSettings();
};

#endif // _settingsmanager_h