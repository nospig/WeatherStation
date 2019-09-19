#ifndef _settingsmanager_h
#define _settingsmanager_h

#include <ArduinoJson.h>
#include "DisplayBase.h"

const String SETTINGS_FILE_NAME = "Settings.json";

typedef struct SettingsData
{
    int thingSpeakChannelID;
    String thingSpeakAPIKey;
    
    String openWeatherMapAPIKey;
    String openWeatherLocationID;

    DisplayMode dispayMode;
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

        bool getSettingsChanged();
        void resetSettingsChanged();

    private:
        SettingsData data;
        bool settingsChanged;

        void loadSettings();
        void saveSettings();
};

#endif // _settingsmanager_h