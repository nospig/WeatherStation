#ifndef _thingspeakreporter_h
#define _thingspeakreporter_h

#include <SettingsManager.h>

class ThingSpeakReporter
{
    public:
        void init(SettingsManager* settingsManager);

        void sendSensorReadings(float temp, float humidity, float pressure);

    private:
        SettingsManager* settingsManager;
};

#endif // _thingspeakreporter_h