#ifndef _settings_h
#define _settings_h

#include <Arduino.h>

// OTA password
#define OTA_MD5_PASSWORD_HASH "3dd7f02976f79aaeb807bdb6dc1b9ef2"

// task scheduler
#define _TASK_SLEEP_ON_IDLE_RUN

// time
#define SECONDS_MULT 1000
#define MINUTES_MULT 60 * SECONDS_MULT
#define HOURS_MULT 60 * SECONDS_MULT

#define TIME_FETCH_INTERVAL 5 * SECONDS_MULT

// weather
#define NUM_FORECASTS 5                             // needs to match maximum required by display modes

#define WIFI_STRENGTH_INTERVAL          10 * SECONDS_MULT
#define SETTINGS_CHANGED_INTERVAL       10 * SECONDS_MULT
#define SCREENGRAB_INTERVAL             10 * SECONDS_MULT
#define WIFI_CONNECTING_DELAY           2 * SECONDS_MULT

// BME280 pins
// TFT pins changed in the library
const int BME_SDA = D2;
const int BME_SCL = D1;
const uint8_t BME_ADDRESS = 0x76;

#endif // _settings_h