#ifndef _BME_Reader_h
#define _BME_Reader_h

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

class BMEReader
{
    public:
        void init(int sda, int scl, uint8_t address);
        boolean isActive();

        float readTemp();
        float readPressurehPA();
        float readHumidity();

    private:
        unsigned status = 0;
        Adafruit_BME280 bme280;
        TwoWire bmeWire;
};

#endif // _BME_Reader_h