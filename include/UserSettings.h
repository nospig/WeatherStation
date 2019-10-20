#ifndef _user_settings_h
#define _user_settings_h

#include <Arduino.h>

// OTA password
#define OTA_MD5_PASSWORD_HASH "5f4dcc3b5aa765d61d8327deb882cf99"

// BME280 pins
// TFT pins changed in the library
const int BME_SDA = D2;
const int BME_SCL = D1;
const uint8_t BME_ADDRESS = 0x76;

// TFT settings

// change depending if your screen is pins up or down, 0 is usually down, 2 for up
#define SCREEN_ROTATION 2

// Pin used for brightness control. Just connect LED pin on TFT to 3.3V if not required.
#define BRIGHTNESS_PIN          16   // D0


#endif // _user_settings_h