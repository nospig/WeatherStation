#include "ThingSpeakReporter.h"
#include <WiFiClient.h>
#include <ThingSpeak.h>

WiFiClient client;

void ThingSpeakReporter::init(SettingsManager* settingsManager)
{
    this->settingsManager = settingsManager;
    
    ThingSpeak.begin(client);
}

void ThingSpeakReporter::sendSensorReadings(float temp, float humidity, float pressure)
{
    ThingSpeak.setField(1, temp);
    ThingSpeak.setField(2, humidity);
    ThingSpeak.setField(3, pressure);

    ThingSpeak.writeFields(settingsManager->getThingSpeakChannelID(), settingsManager->getThingSpeakApiKey().c_str());
}
