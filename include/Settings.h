#include <Arduino.h>

// task scheduler
#define _TASK_SLEEP_ON_IDLE_RUN

// time
#define SECONDS_MULT 1000
#define MINUTES_MULT 60 * SECONDS_MULT
#define HOURS_MULT 60 * SECONDS_MULT

const long utcOffsetInSeconds = 25200;
#define TIME_FETCH_INTERVAL 5 * SECONDS_MULT

// weather
const String OPEN_WEATHER_MAP_LOCATION_ID = "6845590";
#define NUM_FORECASTS 6

#define CURRENT_WEATHER_INTERVAL 1 * MINUTES_MULT   // for testing, increase for final or config through webserver
#define FORECAST_WEATHER_INTERVAL 5 * MINUTES_MULT   // for testing, increase for final or config through webserver
#define SENSOR_READING_INTERVAL 30 * SECONDS_MULT
#define THINGSPEAK_REPORTING_INTERVAL 30 * MINUTES_MULT
#define WIFI_STRENGTH_INTERVAL 10 * SECONDS_MULT

#define WIFI_CONNECTING_DELAY 2 * SECONDS_MULT

const int BME_SDA = D2;
const int BME_SCL = D1;
const uint8_t BME_ADDRESS = 0x76;
