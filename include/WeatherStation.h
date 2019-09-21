#ifndef _weatherstation_h
#define _weatherstation_h

#include "DisplayBase.h"

// task callbacks
void connectWifiCallback();
void getTimeCallback();
void getCurrentWeatherCallback();
void getWeatherForecastCallback();
void readSensorsCallback();
void updateThingSpeakCallback();
void updateWifiStrengthCallback();
void checkSettingsChangedCallback();
void checkScreenGrabCallback();

void changeDisplayMode(DisplayMode newMode);

void setupOtaUpdates();

#endif // _weatherstation_h