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
 */

#pragma once

#include <Arduino.h>

typedef struct OpenWeatherMapCurrentData
{
    uint16_t weatherId;
    String main;
    String description;
    String icon;
    float temp;
    uint16_t pressure;
    uint8_t humidity;
    float windSpeed;
    float windDeg;
    uint32_t observationTime;
} OpenWeatherMapCurrentData;


class OpenWeatherMapCurrent
{
    public:
        OpenWeatherMapCurrent();
        void updateCurrent(OpenWeatherMapCurrentData *data, String appId, String location);
        void updateCurrentById(OpenWeatherMapCurrentData *data, String appId, String locationId);

        void setMetric(boolean metric) { this->metric = metric; }
        boolean isMetric() { return metric; }

        void setLanguage(String language) { this->language = language; }
        String getLanguage() { return language; }

        String getMeteoconIcon(String icon);

        boolean isValidData() { return validData; }
        void setValidData(boolean valid) { validData = valid; }

    private:
        boolean metric = true;
        String language;
        boolean validData;

        void doUpdate(OpenWeatherMapCurrentData *data, String url);
        String buildUrl(String appId, String locationParameter);
        void deserializeWeather(OpenWeatherMapCurrentData *data, String json);
};
