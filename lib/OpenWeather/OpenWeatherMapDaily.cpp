// Based on work by ThingPulse Ltd., https://thingpulse.com

#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "OpenWeatherMapDaily.h"
#include <ArduinoJson.h>

OpenWeatherMapDaily::OpenWeatherMapDaily(int maxForecasts)
{
    this->maxForecasts = maxForecasts;
    this->forecastWeather = new OpenWeatherMapDailyData[maxForecasts];
}

void OpenWeatherMapDaily::update(String appId, String location)
{
    doUpdate(buildUrl(appId, "q=" + location));
}

void OpenWeatherMapDaily::updateById(String appId, String locationId)
{
    doUpdate(buildUrl(appId, "id=" + locationId));
}

String OpenWeatherMapDaily::buildUrl(String appId, String locationParameter)
{
     // something is caching data, don't know where, try a random param
    int randomForCache = random(2147483647);

    String units = metric ? "metric" : "imperial";
    return "http://api.openweathermap.org/data/2.5/forecast/daily?" + locationParameter + "&appid=" + appId + "&units=" + units + "&lang=" 
        + language + "&cnt=" + maxForecasts + "&nospig=" + randomForCache;
}

void OpenWeatherMapDaily::doUpdate(String url)
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

void OpenWeatherMapDaily::deserializeWeather(String json)
{
    //Serial.println(json);

    DynamicJsonDocument doc(4096); // size calculated with ArduinoJson assistant, enough for 6 forecasts
    deserializeJson(doc, json);

    numForcasts = doc["cnt"];
    uint32_t timeZone = doc["city"]["timezone"];

    //Serial.printf("Timezone : %d\n",timeZone);

    JsonArray list = doc["list"];

    for(int i=0; i<numForcasts; i++)
    {
        OpenWeatherMapDailyData* data = &forecastWeather[i];
        JsonObject day = list[i];

        data->time = day["dt"];

        JsonObject temp = day["temp"];

        data->tempMin = temp["min"];
        data->tempMax = temp["max"];

        JsonObject weather = day["weather"][0];

        data->icon = (const char*)weather["icon"];
        data->description = (const char*)weather["description"];
        data->description = captaliseString(data->description);
        data->timeZone = timeZone;
    }
}

OpenWeatherMapDailyData* OpenWeatherMapDaily::getDailyForecasts()
{
    return forecastWeather;
}

int OpenWeatherMapDaily::getForecastCount()
{
    return numForcasts;
}

String OpenWeatherMapDaily::captaliseString(String input)
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