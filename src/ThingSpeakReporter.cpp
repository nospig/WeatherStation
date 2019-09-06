#include "ThingSpeakReporter.h"
#include "Secrets.h"
#include <WiFiClient.h>
#include <ThingSpeak.h>

WiFiClient client;

void ThingSpeakReporter::init()
{
    ThingSpeak.begin(client);
}

void ThingSpeakReporter::sendSensorReadings(float temp, float humidity, float pressure)
{
    ThingSpeak.setField(1, temp);
    ThingSpeak.setField(2, humidity);
    ThingSpeak.setField(3, pressure);

    ThingSpeak.writeFields(THINGSPEAK_SECRET_CH_ID, THINGSPEAK_SECRET_WRITE_APIKEY);
}
