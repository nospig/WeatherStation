#include <Arduino.h>
#include <ArduinoJson.h>
#include <FS.h>
#include "SettingsManager.h"


void SettingsManager::init()
{
    // for testing now as settings not saved to start with
    //SPIFFS.remove(SETTINGS_FILE_NAME);

    if(!SPIFFS.exists(SETTINGS_FILE_NAME))
    {
        //Serial.println("No settings found, creating.");
        setDefaultSettings();
        saveSettings();
    }
    
    loadSettings();

    settingsChanged = false;
}

void SettingsManager::setDefaultSettings()
{
    data.dispayMode = DisplayMode_1;
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

    int mode = doc["DisplayMode"];
    data.dispayMode = (DisplayMode)mode;

    // testing
    //serializeJson(doc, Serial);
    //Serial.println();
}

void SettingsManager::saveSettings()
{
    File jsonSettings;
    DynamicJsonDocument doc(512);       // TODO, get correct size

    doc["WeatherAPIKey"] = data.openWeatherMapAPIKey;
    doc["WeatherLLocationID"] = data.openWeatherLocationID;
    doc["ThingSpeakChannelID"] = data.thingSpeakChannelID;
    doc["ThingSpeakAPIKey"] = data.thingSpeakAPIKey;
    doc["DisplayMode"] = (int)data.dispayMode;

    jsonSettings = SPIFFS.open(SETTINGS_FILE_NAME, "w");
    if(jsonSettings)
    {
        serializeJson(doc, jsonSettings);
        //Serial.println("Settings file saved.");
        jsonSettings.close();
    }
    else
    {
        Serial.println("Unable to save settings file.");
    }
    
    // testing
    //serializeJson(doc, Serial); 
    //Serial.println();
}


String SettingsManager::getThingSpeakApiKey()
{
    return data.thingSpeakAPIKey;
}

void SettingsManager::setThingSpeakApiKey(String apiKey)
{
    if(apiKey != data.thingSpeakAPIKey)
    {
        data.thingSpeakAPIKey = apiKey;
        saveSettings();
        settingsChanged = true;
    }
}

int SettingsManager::getThingSpeakChannelID()
{
    return data.thingSpeakChannelID;
}

void SettingsManager::setThingSpeakChannelID(int channelID)
{
    if(channelID != data.thingSpeakChannelID)
    {
        data.thingSpeakChannelID = channelID;
        saveSettings();
        settingsChanged = true;
    }
}

String SettingsManager::getOpenWeatherApiKey()
{
    return data.openWeatherMapAPIKey;
}

void SettingsManager::setOpenWeatherApiKey(String apiKey)
{
    if(apiKey != data.openWeatherMapAPIKey)
    {
        data.openWeatherMapAPIKey = apiKey;
        saveSettings();
        settingsChanged = true;
    }
}

String SettingsManager::getOpenWeatherlocationID()
{
    return data.openWeatherLocationID;
}

void SettingsManager::setOpenWeatherlocationID(String locationID)
{
    if(locationID != data.openWeatherLocationID)
    {
        data.openWeatherLocationID = locationID;
        saveSettings();
        settingsChanged = true;
    }
}

DisplayMode SettingsManager::getDisplayMode()
{
    return data.dispayMode;
}

void SettingsManager::setDisplayMode(DisplayMode displayMode)
{
    if(displayMode != data.dispayMode)
    {
        data.dispayMode = displayMode;
        saveSettings();
        settingsChanged = true;
    }
}


bool SettingsManager::getSettingsChanged()
{
    return settingsChanged;
}

void SettingsManager::resetSettingsChanged()
{
    settingsChanged = false;
}

