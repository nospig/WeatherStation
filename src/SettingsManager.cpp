#include <Arduino.h>
#include <ArduinoJson.h>
#include <FS.h>
#include "SettingsManager.h"
#include "Settings.h"
#include "Secrets.h"

void SettingsManager::init()
{
    // for testing now as settings not saved to start with
    //SPIFFS.remove(SETTINGS_FILE_NAME);


    if(!SPIFFS.exists(SETTINGS_FILE_NAME))
    {
        Serial.println("No settings found, creating.");
        setDefaultSettings();
        saveSettings();
    }
    
    loadSettings();

    settingsChanged = false;
}

void SettingsManager::setDefaultSettings()
{
    // TODO, maybe blank these to force setup before use?
    data.thingSpeakChannelID = THINGSPEAK_SECRET_CH_ID;
    data.thingSpeakAPIKey = THINGSPEAK_SECRET_WRITE_APIKEY;
    data.openWeatherMapAPIKey = OPEN_WEATHER_MAP_APP_ID;
    data.openWeatherLocationID = OPEN_WEATHER_MAP_LOCATION_ID;
}

void SettingsManager::loadSettings()
{
    File jsonSettings;
    DynamicJsonDocument doc(512);       // TODO, get correct size

    jsonSettings = SPIFFS.open(SETTINGS_FILE_NAME, "r");
    deserializeJson(doc, jsonSettings);

    data.openWeatherMapAPIKey = (const char*)doc["WeatherAPIKey"];
    data.openWeatherLocationID = (const char*)doc["WeatherLLocationID"];
    data.thingSpeakChannelID = doc["ThingSpeakChannelID"];
    data.thingSpeakAPIKey = (const char*)doc["ThingSpeakAPIKey"];

    // testing
    serializeJson(doc, Serial);
    Serial.println();
}

void SettingsManager::saveSettings()
{
    File jsonSettings;
    DynamicJsonDocument doc(512);       // TODO, get correct size

    doc["WeatherAPIKey"] = data.openWeatherMapAPIKey;
    doc["WeatherLLocationID"] = data.openWeatherLocationID;
    doc["ThingSpeakChannelID"] = data.thingSpeakChannelID;
    doc["ThingSpeakAPIKey"] = data.thingSpeakAPIKey;

    jsonSettings = SPIFFS.open(SETTINGS_FILE_NAME, "w");
    if(jsonSettings)
    {
        serializeJson(doc, jsonSettings);
        Serial.println("Settings file saved.");
        jsonSettings.close();
    }
    else
    {
        Serial.println("Unable to save settings file.");
    }
    
     // testing
    serializeJson(doc, Serial); 
    Serial.println();
}


String SettingsManager::getThingSpeakApiKey()
{
    return data.thingSpeakAPIKey;
}

void SettingsManager::setThingSpeakApiKey(String apiKey)
{
    data.thingSpeakAPIKey = apiKey;
    settingsChanged = true;
}

int SettingsManager::getThingSpeakChannelID()
{
    return data.thingSpeakChannelID;
}

void SettingsManager::setThingSpeakChannelID(int channelID)
{
    data.thingSpeakChannelID = channelID;
    settingsChanged = true;
}

String SettingsManager::getOpenWeatherApiKey()
{
    return data.openWeatherMapAPIKey;
}

void SettingsManager::setOpenWeatherApiKey(String apiKey)
{
    data.openWeatherMapAPIKey = apiKey;
    settingsChanged = true;
}

String SettingsManager::getOpenWeatherlocationID()
{
    return data.openWeatherLocationID;
}

void SettingsManager::setOpenWeatherlocationID(String locationID)
{
    data.openWeatherLocationID = locationID;
    settingsChanged = true;
}

bool SettingsManager::getSettingsChanged()
{
    return settingsChanged;
}

void SettingsManager::resetSettingsChanged()
{
    settingsChanged = false;
}

