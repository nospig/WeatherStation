
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "OpenWeatherMapDaily.h"
#include <ArduinoJson.h>

OpenWeatherMapDaily::OpenWeatherMapDaily()
{
}

void OpenWeatherMapDaily::update(OpenWeatherMapDaliyData *data, String appId, String location, uint8_t maxForecasts)
{
    this->maxForecasts = maxForecasts;
    doUpdate(data, buildUrl(appId, "q=" + location));
}

void OpenWeatherMapDaily::updateById(OpenWeatherMapDaliyData *data, String appId, String locationId, uint8_t maxForecasts)
{
    this->maxForecasts = maxForecasts;
    doUpdate(data, buildUrl(appId, "id=" + locationId));
}

String OpenWeatherMapDaily::buildUrl(String appId, String locationParameter)
{
    String units = metric ? "metric" : "imperial";
    return "http://api.openweathermap.org/data/2.5/forecast/daily?" + locationParameter + "&appid=" + appId + "&units=" + units + "&lang=" + language + "&cnt=" + maxForecasts;
}

void OpenWeatherMapDaily::doUpdate(OpenWeatherMapDaliyData *data, String url)
{
    this->setValidData(false);

    // must be in this order
    WiFiClient client;
    HTTPClient http;

    http.begin(client, url);
    int httpCode = http.GET();

    if (httpCode > 0)
    {
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
        {
            deserializeWeather(data, http.getString());
        }
    }
    http.end();
}

void OpenWeatherMapDaily::deserializeWeather(OpenWeatherMapDaliyData *data, String json)
{
    //Serial.println(json);

    DynamicJsonDocument doc(4096); // size calculated with ArduinoJson assistant, enough for 6 forecasts
    deserializeJson(doc, json);

/*
    JsonObject weather_0 = doc["weather"][0];
    data->main = (const char*)weather_0["main"];
    data->description = (const char*)weather_0["description"];
    data->icon = (const char*)weather_0["icon"];

    data->windSpeed = doc["wind"]["speed"];
    data->windDeg = doc["wind"]["deg"]; 
    data->observationTime = doc["dt"];

    JsonObject main = doc["main"];
    data->temp = main["temp"];
    data->pressure = main["pressure"];
    data->humidity = main["humidity"];   

    */
}

String OpenWeatherMapDaily::getMeteoconIcon(String icon)
{
    // clear sky
    // 01d
    if (icon == "01d")
    {
        return "B";
    }
    // 01n
    if (icon == "01n")
    {
        return "C";
    }
    // few clouds
    // 02d
    if (icon == "02d")
    {
        return "H";
    }
    // 02n
    if (icon == "02n")
    {
        return "4";
    }
    // scattered clouds
    // 03d
    if (icon == "03d")
    {
        return "N";
    }
    // 03n
    if (icon == "03n")
    {
        return "5";
    }
    // broken clouds
    // 04d
    if (icon == "04d")
    {
        return "Y";
    }
    // 04n
    if (icon == "04n")
    {
        return "%";
    }
    // shower rain
    // 09d
    if (icon == "09d")
    {
        return "R";
    }
    // 09n
    if (icon == "09n")
    {
        return "8";
    }
    // rain
    // 10d
    if (icon == "10d")
    {
        return "Q";
    }
    // 10n
    if (icon == "10n")
    {
        return "7";
    }
    // thunderstorm
    // 11d
    if (icon == "11d")
    {
        return "P";
    }
    // 11n
    if (icon == "11n")
    {
        return "6";
    }
    // snow
    // 13d
    if (icon == "13d")
    {
        return "W";
    }
    // 13n
    if (icon == "13n")
    {
        return "#";
    }
    // mist
    // 50d
    if (icon == "50d")
    {
        return "M";
    }
    // 50n
    if (icon == "50n")
    {
        return "M";
    }
    // Nothing matched: N/A
    return ")";
}
