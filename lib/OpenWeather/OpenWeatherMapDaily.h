#ifndef _openweathermapdaily_h
#define _openweathermapdaily_h

#include <Arduino.h>

typedef struct OpenWeatherMapDaliyData
{
    float tempMin;
    float tempMax;
    String icon;
    String description;

    //String main;
    //uint16_t pressure;
    //uint8_t humidity;
    //float windSpeed;
    //float windDeg;
    //uint32_t observationTime;
} OpenWeatherMapDailyData;


class OpenWeatherMapDaily
{
    public:
        OpenWeatherMapDaily(int maxForecasts);

        void update(String appId, String location);
        void updateById(String appId, String locationId);

        void setMetric(boolean metric) { this->metric = metric; }
        boolean isMetric() { return metric; }

        void setLanguage(String language) { this->language = language; }
        String getLanguage() { return language; }

        boolean isValidData() { return validData; }
        void setValidData(boolean valid) { validData = valid; }

        OpenWeatherMapDailyData* getDailyForecasts();
        int getForecastCount();

    private:
        boolean metric = true;
        String language;
        boolean validData;
        int maxForecasts;
        int numForcasts;
        OpenWeatherMapDailyData* forecastWeather;

        void doUpdate(String url);
        String buildUrl(String appId, String locationParameter);
        void deserializeWeather(String json);
        String captaliseString(String input);
};


#endif // _openweathermapdaily_h