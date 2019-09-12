#include <Arduino.h>
#include "BMEReader.h"

void BMEReader::init(int sda, int scl, uint8_t address)
{
    bmeWire.begin(sda, scl);
    status = bme280.begin(address, &bmeWire);

    if(!status)
    {
        Serial.println("BME280 not started!");
    }
}

float BMEReader::readTemp()
{
    return bme280.readTemperature();
}

float BMEReader::readPressurehPA()
{
    return (bme280.readPressure() / 100.f);
}

float BMEReader::readHumidity()
{
    return bme280.readHumidity();
}

boolean BMEReader::isActive()
{
    return status != 0;
}
