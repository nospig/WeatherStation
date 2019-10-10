#ifndef _user_settings_h
#define _user_settings_h

#include <Arduino.h>

// OTA password
#define OTA_MD5_PASSWORD_HASH "3dd7f02976f79aaeb807bdb6dc1b9ef2"

// BME280 pins
// TFT pins changed in the library
const int BME_SDA = D2;
const int BME_SCL = D1;
const uint8_t BME_ADDRESS = 0x76;

// change depending if your screen is pins up or down, 0 is usually down, 2 for up
#define SCREEN_ROTATION 2


#endif // _user_settings_h