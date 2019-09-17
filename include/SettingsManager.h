#ifndef _settingsmanager_h
#define _settingsmanager_h

#include <ArduinoJson.h>

const String SETTINGS_FILE_NAME = "Settings.json";

typedef struct SettingsData
{
    int thingSpeakChannelID;
    String thingSpeakAPIKey;
    
    String openWeatherMapAPIKey;
    String openWeatherLocationID;
} SettingsData;

class SettingsManager
{
    public:
        void init();

        String getThingSpeakApiKey();
        void setThingSpeakApiKey(String apiKey);

        int getThingSpeakChannelID();
        void setThingSpeakChannelID(int channelID);

        String getOpenWeatherApiKey();
        void setOpenWeatherApiKey(String apiKey);

        String getOpenWeatherlocationID();
        void setOpenWeatherlocationID(String locationID);

        bool getSettingsChanged();
        void resetSettingsChanged();

    private:
        SettingsData data;
        bool settingsChanged;

        void setDefaultSettings();
        void loadSettings();
        void saveSettings();
};

#endif // _settingsmanager_h