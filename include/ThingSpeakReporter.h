#ifndef _thingspeakreporter_h
#define _thingspeakreporter_h

class ThingSpeakReporter
{
    public:
        void init();

        void sendSensorReadings(float temp, float humidity, float pressure);
};

#endif // _thingspeakreporter_h