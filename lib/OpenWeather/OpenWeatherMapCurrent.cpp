/**The MIT License (MIT)
 
 Copyright (c) 2018 by ThingPulse Ltd., https://thingpulse.com
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 
 File modified from the ThingPulse project to change json decoding and data saved
 */

#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "OpenWeatherMapCurrent.h"
#include <ArduinoJson.h>

OpenWeatherMapCurrent::OpenWeatherMapCurrent()
{
}

OpenWeatherMapCurrentData* OpenWeatherMapCurrent::getCurrentData()
{
    return &data;
}

void OpenWeatherMapCurrent::update(String appId, String location)
{
    doUpdate(buildUrl(appId, "q=" + location));
}

void OpenWeatherMapCurrent::updateById(String appId, String locationId)
{
    doUpdate(buildUrl(appId, "id=" + locationId));
}

String OpenWeatherMapCurrent::buildUrl(String appId, String locationParameter)
{
    String units = metric ? "metric" : "imperial";
    return "http://api.openweathermap.org/data/2.5/weather?" + locationParameter + "&appid=" + appId + "&units=" + units + "&lang=" + language;
}

void OpenWeatherMapCurrent::doUpdate(String url)
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
            deserializeWeather(http.getString());
        }
    }
    http.end();
}

void OpenWeatherMapCurrent::deserializeWeather(String json)
{
    //Serial.println(json);

    DynamicJsonDocument doc(1024); // size calculated with ArduinoJson assistant
    deserializeJson(doc, json);

    JsonObject weather_0 = doc["weather"][0];
    data.main = (const char*)weather_0["main"];
    data.description = (const char*)weather_0["description"];
    data.icon = (const char*)weather_0["icon"];

    data.windSpeed = doc["wind"]["speed"];
    data.windDeg = doc["wind"]["deg"]; 
    data.observationTime = doc["dt"];

    JsonObject main = doc["main"];
    data.temp = main["temp"];
    data.pressure = main["pressure"];
    data.humidity = main["humidity"];   
}

String OpenWeatherMapCurrent::getMeteoconIcon(String icon)
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
