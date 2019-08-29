#include <Arduino.h>

// time
#define SECONDS_MULT 1000
#define MINUTES_MULT 60 * SECONDS_MULT

const long utcOffsetInSeconds = 25200;
const char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
#define TIME_FETCH_INTERVAL 5 * SECONDS_MULT

// weather
const String OPEN_WEATHER_MAP_APP_ID = "6ab13e460fb4109fea6810327ec66ff1";
const String OPEN_WEATHER_MAP_LOCATION_ID = "6845590";