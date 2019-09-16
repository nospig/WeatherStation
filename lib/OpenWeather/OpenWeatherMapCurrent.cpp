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
    // something is caching data, don't know where, try a random param
    int randomForCache = random(2147483647);
    return "http://api.openweathermap.org/data/2.5/weather?" + locationParameter + "&appid=" + appId + "&units=" + units + "&lang=" + language + "&nospig=" + randomForCache;
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
    data.timeZone = doc["timeZone"];
    data.location = (const char*)doc["name"];

    JsonObject main = doc["main"];
    data.temp = main["temp"];
    data.tempMin = main["temp_min"];
    data.tempMax = main["temp_max"];
    data.pressure = main["pressure"];
    data.humidity = main["humidity"];   
    
    data.description = captaliseString(data.description);
    data.main = captaliseString(data.main);

    JsonObject clouds = doc["clouds"];
    if(clouds.isNull())
    {
        data.cloudPercentage = -1;
        Serial.println("NO clouds");
    }
    else
    {
        data.cloudPercentage = clouds["all"];
        Serial.println("Has clouds");
    }

    JsonObject rain = doc["rain"];
    if(rain.isNull())
    {
        data.rainOneHour = -1;
        data.rainThreeHour = -1;
        Serial.println("NO rain");
    }
    else
    {
        data.rainOneHour = rain["1h"];
        data.rainThreeHour = rain["3h"];
        Serial.println("Has rain");
    }
}

String OpenWeatherMapCurrent::captaliseString(String input)
{
    char last = ' ';

    input.toLowerCase();

    for(unsigned int i=0; i<input.length(); i++)
    {
        char c = input.charAt(i);
        if(last == ' ' && c != ' ' && isAlpha(c))
        {
            input.setCharAt(i, toUpperCase(c));            
        }
        last = c;
    }

    return input;
}
