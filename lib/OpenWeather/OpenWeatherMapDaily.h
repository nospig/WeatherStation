#ifndef _openweathermapdaily_h
#define _openweathermapdaily_h

#include <Arduino.h>

typedef struct OpenWeatherMapDaliyData
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
} OpenWeatherMapDailyData;


class OpenWeatherMapDaily
{
    public:
        OpenWeatherMapDaily();
        void update(OpenWeatherMapDailyData *data, String appId, String location, uint8_t maxForecasts);
        void updateById(OpenWeatherMapDailyData *data, String appId, String locationId, uint8_t maxForecasts);

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
        uint8_t maxForecasts;

        void doUpdate(OpenWeatherMapDailyData *data, String url);
        String buildUrl(String appId, String locationParameter);
        void deserializeWeather(OpenWeatherMapDailyData *data, String json);
};


#endif // _openweathermapdaily_h