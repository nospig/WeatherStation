#include <Arduino.h>
#include <ArduinoJson.h>
#include <FS.h>
#include "SettingsManager.h"

const int CURRENT_WEATHER_INTERVAL      = 10 * MINUTES_MULT;
const int FORECAST_WEATHER_INTERVAL     = 25 * MINUTES_MULT; 
const int SENSOR_READING_INTERVAL       = 30 * SECONDS_MULT;
const int THINGSPEAK_REPORTING_INTERVAL = 30 * MINUTES_MULT;
const int MQTT_PUBLISH_INTERVAL         = 5 * MINUTES_MULT;
const int PRINT_MONITOR_INERVAL         = 30 * SECONDS_MULT;

void SettingsManager::init()
{
    // for testing now as settings not saved to start with
    //SPIFFS.remove(SETTINGS_FILE_NAME);

    if(!SPIFFS.exists(SETTINGS_FILE_NAME))
    {
        //Serial.println("No settings found, creating.");
        resetSettings();
        saveSettings();
    }
    
    loadSettings();

    settingsChanged = false;
    mqttReconnectRequired = false;
}

void SettingsManager::resetSettings()
{
    data.thingSpeakChannelID = 0;
    data.thingSpeakAPIKey = "";
    data.openWeatherMapAPIKey = "";
    data.openWeatherLocationID = "";

    data.dispayMode = DisplayMode_1;
    data.displayBrightness = 100;
    data.displayMetric = true;

    data.currentWeatherInterval = CURRENT_WEATHER_INTERVAL;
    data.forecastWeatherInterval = FORECAST_WEATHER_INTERVAL;
    data.sensorReadingInterval = SENSOR_READING_INTERVAL;
    data.thingSpeakReportingInterval = THINGSPEAK_REPORTING_INTERVAL;
    data.mqttPublishInterval = MQTT_PUBLISH_INTERVAL;
    data.printMonitorInterval = PRINT_MONITOR_INERVAL;

    data.thingSpeakEnabled = false;
    data.mqttEnabled = false;

    data.mqttBroker = "";
    data.mqttUsername = "";
    data.mqttPassword = "";
    data.mqttClientId = "";
    data.mqttTempTopic = "";
    data.mqttHumidityTopic = "";
    data.mqttPressureTopic = "";
    data.mqttDisplayTopic = "";
    data.mqttPort = 1883;

    data.octoPrintAddress = "";
    data.octoPrintPort = 80;
    data.octoPrintUsername = "";
    data.octoPrintPassword = "";
    data.octoPrintAPIKey = "";
    data.octoPrintMonitorEnabled = false;

    data.utcOffsetSeconds = 0;

    saveSettings();
    settingsChanged = true;
}

void SettingsManager::loadSettings()
{
    File jsonSettings;
    DynamicJsonDocument doc(SETTINGS_JSON_SIZE);

    jsonSettings = SPIFFS.open(SETTINGS_FILE_NAME, "r");
    deserializeJson(doc, jsonSettings);

    data.openWeatherMapAPIKey = (const char*)doc["WeatherAPIKey"];
    data.openWeatherLocationID = (const char*)doc["WeatherLLocationID"];
    data.thingSpeakChannelID = doc["ThingSpeakChannelID"];
    data.thingSpeakAPIKey = (const char*)doc["ThingSpeakAPIKey"];

    int mode = doc["DisplayMode"];
    data.dispayMode = (DisplayMode)mode;
    data.displayBrightness = doc["DisplayBrightness"];
    data.displayMetric = doc["DisplayMetric"];

    data.currentWeatherInterval = doc["CurrentWeatherInterval"];
    data.forecastWeatherInterval = doc["ForecastWeatherInterval"];
    data.sensorReadingInterval = doc["SensorReadingInterval"];
    data.thingSpeakReportingInterval = doc["ThingSpeakReportingInterval"];
    data.mqttPublishInterval = doc["MqttPublishInterval"];
    data.printMonitorInterval = doc["PrinterMonitorInterval"];

    data.thingSpeakEnabled = doc["ThingSpeakEnabled"];
    data.mqttEnabled = doc["MqttEnabled"];

    data.mqttBroker = (const char*)doc["MqttBroker"];
    data.mqttUsername = (const char*)doc["MqttUsername"];
    data.mqttPassword = (const char*)doc["MqttPassword"];
    data.mqttClientId = (const char*)doc["MqttClientID"];
    data.mqttTempTopic = (const char*)doc["MqttTempTopic"];
    data.mqttHumidityTopic = (const char*)doc["MqttHumidityTopic"];
    data.mqttPressureTopic = (const char*)doc["MqttPressureTopic"];
    data.mqttDisplayTopic = (const char*)doc["MqttDisplayTopic"];    
    data.mqttPort = doc["MqttPort"];
    
    data.octoPrintAddress = (const char*)doc["OctoPrintAddress"];
    data.octoPrintPort = doc["OctoPrintPort"];
    data.octoPrintUsername = (const char*)doc["OctoPrintUsername"];
    data.octoPrintPassword = (const char*)doc["OctoPrintPassword"];
    data.octoPrintAPIKey = (const char*)doc["OctoPrintAPIKey"];
    data.octoPrintMonitorEnabled = doc["OctoPrintEnabled"];

    data.utcOffsetSeconds = doc["utcOffset"];

    // testing
    Serial.println();
    serializeJson(doc, Serial);
    Serial.println();
}

void SettingsManager::saveSettings()
{
    File jsonSettings;
    DynamicJsonDocument doc(SETTINGS_JSON_SIZE);   

    doc["WeatherAPIKey"] = data.openWeatherMapAPIKey;
    doc["WeatherLLocationID"] = data.openWeatherLocationID;
    doc["ThingSpeakChannelID"] = data.thingSpeakChannelID;
    doc["ThingSpeakAPIKey"] = data.thingSpeakAPIKey;
    doc["DisplayMode"] = (int)data.dispayMode;
    doc["DisplayBrightness"] = data.displayBrightness;
    doc["DisplayMetric"] = data.displayMetric;
    doc["CurrentWeatherInterval"] = data.currentWeatherInterval;
    doc["ForecastWeatherInterval"] = data.forecastWeatherInterval;
    doc["SensorReadingInterval"] = data.sensorReadingInterval;
    doc["ThingSpeakReportingInterval"] = data.thingSpeakReportingInterval;
    doc["ThingSpeakEnabled"] = data.thingSpeakEnabled;
    doc["MqttPublishInterval"] = data.mqttPublishInterval;
    doc["PrinterMonitorInterval"] = data.printMonitorInterval;
    doc["MqttEnabled"] = data.mqttEnabled;
    doc["MqttBroker"] = data.mqttBroker;
    doc["MqttUsername"] = data.mqttUsername;
    doc["MqttPassword"] = data.mqttPassword;
    doc["MqttClientID"] = data.mqttClientId;
    doc["MqttTempTopic"] = data.mqttTempTopic;
    doc["MqttHumidityTopic"] = data.mqttHumidityTopic;
    doc["MqttPressureTopic"] = data.mqttPressureTopic;
    doc["MqttDisplayTopic"] = data.mqttDisplayTopic;
    doc["MqttPort"] = data.mqttPort;
    doc["utcOffset"] = data.utcOffsetSeconds;

    doc["OctoPrintAddress"] = data.octoPrintAddress;
    doc["OctoPrintPort"] = data.octoPrintPort;
    doc["OctoPrintUsername"] = data.octoPrintUsername;
    doc["OctoPrintPassword"] = data.octoPrintPassword;
    doc["OctoPrintAPIKey"] = data.octoPrintAPIKey;
    doc["OctoPrintEnabled"] = data.octoPrintMonitorEnabled;

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

void SettingsManager::updateSettings()
{
    saveSettings();
    settingsChanged = true;
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
        updateSettings();
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
        updateSettings();
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
        updateSettings();
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
        updateSettings();
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
        updateSettings();
    }
}

bool SettingsManager::getDisplayMetric()
{
    return data.displayMetric;
}

void SettingsManager::setDisplayMetric(bool metric)
{
    if(metric != data.displayMetric)
    {
        data.displayMetric = metric;
        updateSettings();
    }
}

int SettingsManager::getCurrentWeatherInterval()
{
    return data.currentWeatherInterval;
}

void SettingsManager::setCurrentWeatherInterval(int interval)
{
    if(data.currentWeatherInterval != interval)
    {
        data.currentWeatherInterval = interval;
        updateSettings();
    }
}

int SettingsManager::getForecastWeatherInterval()
{
    return data.forecastWeatherInterval;
}

void SettingsManager::setForecastWeatherInterval(int interval)
{
    if(data.forecastWeatherInterval != interval)
    {
        data.forecastWeatherInterval = interval;
        updateSettings();
    }
}

int SettingsManager::getSensorReadingInterval()
{
    return data.sensorReadingInterval;
}

void SettingsManager::setSensorReadingInterval(int interval)
{
    if(data.sensorReadingInterval != interval)
    {
        data.sensorReadingInterval = interval;
        updateSettings();
    }
}

int SettingsManager::getThingSpeakReportingInterval()
{
    return data.thingSpeakReportingInterval;
}

void SettingsManager::setThingSpeakReportingInterval(int interval)
{
    if(data.thingSpeakReportingInterval != interval)
    {
        data.thingSpeakReportingInterval = interval;
        updateSettings();
    }
}

bool SettingsManager::getThingSpeakEnabled()
{
    return data.thingSpeakEnabled;
}

void SettingsManager::setThingSpeakEnabled(bool enabled)
{
    if(data.thingSpeakEnabled != enabled)
    {
        data.thingSpeakEnabled = enabled;
        updateSettings();
    }
}

int SettingsManager::getMqttPublishInterval()
{
    return data.mqttPublishInterval;
}

void SettingsManager::setMqttPublishInterval(int interval)
{
    if(data.mqttPublishInterval != interval)
    {
        data.mqttPublishInterval = interval;
        updateSettings();
    }
}

int SettingsManager::getPrintMonitorInterval()
{
    return data.printMonitorInterval;
}

void SettingsManager::setPrintMonitorInterval(int interval)
{
    if(data.printMonitorInterval != interval)
    {
        data.printMonitorInterval = interval;
        updateSettings();
    }
}

bool SettingsManager::getMqttEnabled()
{
    return data.mqttEnabled;
}

void SettingsManager::setMqttEnabled(bool enabled)
{
    if(data.mqttEnabled != enabled)
    {
        data.mqttEnabled = enabled;
        updateSettings();

        if(enabled)
        {
            mqttReconnectRequired = true;
        }
    }
}

int SettingsManager::getMqttPort()
{
    return data.mqttPort;
}

void SettingsManager::setMqttPort(int port)
{
    if(data.mqttPort != port)
    {
        data.mqttPort = port;
        updateSettings();
        mqttReconnectRequired = true;
    }
}

String SettingsManager::getMqttBroker()
{
    return data.mqttBroker;
}

void SettingsManager::setMqttBroker(String url)
{
    if(data.mqttBroker != url)
    {
        data.mqttBroker = url;
        updateSettings();
        mqttReconnectRequired = true;
    }
}

String SettingsManager::getMqttUsername()
{
    return data.mqttUsername;
}

void SettingsManager::setMqttUsername(String userName)
{
    if(data.mqttUsername != userName)
    {
        data.mqttUsername = userName;
        updateSettings();
        mqttReconnectRequired = true;
    }
}

String SettingsManager::getMqttPassword()
{
    return data.mqttPassword;
}

void SettingsManager::setMqttPassword(String password)
{
    if(data.mqttPassword != password)
    {
        data.mqttPassword = password;
        updateSettings();
        mqttReconnectRequired = true;
    }
}

String SettingsManager::getMqttClientId()
{
    return data.mqttClientId;
}

void SettingsManager::setMqttClientId(String clientId)
{
    if(data.mqttClientId != clientId)
    {
        data.mqttClientId = clientId;
        updateSettings();
    }
}

String SettingsManager::getMqttTempTopic()
{
    return data.mqttTempTopic;
}

void SettingsManager::setMqttTempTopic(String tempTopic)
{
    if(data.mqttTempTopic != tempTopic)
    {
        data.mqttTempTopic = tempTopic;
        updateSettings();
    }
}

String SettingsManager::getMqttHumidityTopic()
{
    return data.mqttHumidityTopic;
}

void SettingsManager::setMqttHumidityTopic(String humidityTopic)
{
    if(data.mqttHumidityTopic != humidityTopic)
    {
        data.mqttHumidityTopic = humidityTopic;
        updateSettings();
    }
}

String SettingsManager::getMqttPressureTopic()
{
    return data.mqttPressureTopic;
}

void SettingsManager::setMqttPressureTopic(String pressureTopic)
{
    if(data.mqttPressureTopic != pressureTopic)
    {
        data.mqttPressureTopic = pressureTopic;
        updateSettings();
    }
}

String SettingsManager::getMqttDisplayTopic()
{
    return data.mqttDisplayTopic;
}

void SettingsManager::setMqttDisplayTopic(String displayTopic)
{
    if(data.mqttDisplayTopic != displayTopic)
    {
        data.mqttDisplayTopic = displayTopic;
        updateSettings();
    }
}

String SettingsManager::getOctoPrintAddress()
{
    return data.octoPrintAddress;
}

void SettingsManager::setOctoPrintAddress(String address)
{
    if(data.octoPrintAddress != address)
    {
        data.octoPrintAddress = address;
        updateSettings();
    }
}

int SettingsManager::getOctoPrintPort()
{
    return data.octoPrintPort;
}

void SettingsManager::setOctoPrintPort(int port)
{
    if(data.octoPrintPort != port)
    {
        data.octoPrintPort = port;
        updateSettings();
    }
}

String SettingsManager::getOctoPrintUsername()
{
    return data.octoPrintUsername;
}

void SettingsManager::setOctoPrintUsername(String userName)
{
    if(data.octoPrintUsername != userName)
    {
        data.octoPrintUsername = userName;
        updateSettings();
    }
}

String SettingsManager::getOctoPrintPassword()
{
    return data.octoPrintPassword;
}

void SettingsManager::setOctoPrintPassword(String password)
{
    if(data.octoPrintPassword != password)
    {
        data.octoPrintPassword = password;
        updateSettings();
    }
}

String SettingsManager::getOctoPrintAPIKey()
{
    return data.octoPrintAPIKey;
}

void SettingsManager::setOctoPrintAPIKey(String apiKey)
{
    if(data.octoPrintAPIKey != apiKey)
    {
        data.octoPrintAPIKey = apiKey;
        updateSettings();
    }
}

bool SettingsManager::getOctoPrintEnabled()
{
    return data.octoPrintMonitorEnabled;
}

void SettingsManager::setOctoPrintEnabled(bool enabled)
{
    if(data.octoPrintMonitorEnabled != enabled)
    {
        data.octoPrintMonitorEnabled = enabled;
        updateSettings();
    }
}

long SettingsManager::getUtcOffset()
{
    return data.utcOffsetSeconds;
}

void SettingsManager::setUtcOffset(long utcOffset)
{
    if(data.utcOffsetSeconds != utcOffset)
    {
        data.utcOffsetSeconds = utcOffset;
        updateSettings();
    }
}

int SettingsManager::getDisplayBrightness()
{
    return data.displayBrightness;
}

void SettingsManager::setDisplayBrightness(int brightnessPercent)
{
    if(brightnessPercent < 0)
    {
        brightnessPercent = 0;
    }
    if(brightnessPercent > 100)
    {
        brightnessPercent = 100;
    }
     if(data.displayBrightness != brightnessPercent)
    {
        data.displayBrightness = brightnessPercent;
        updateSettings();
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

bool SettingsManager::getMqttReconnectRequired()
{
    return mqttReconnectRequired;
}

void SettingsManager::resetMqttReconnectRequired()
{
    mqttReconnectRequired = false;
}
