#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "OctoPrintMonitor.h"

void OctoPrintMonitor::init(SettingsManager* settingsManager)
{
    this->settingsManager = settingsManager;
}
