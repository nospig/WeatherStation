# Weather Station

Another version of the popular ESP2866 weather station.

Built using the following hardware bought from AliExpress.

[Wemos D1 mini ESP8266 board](https://www.aliexpress.com/item/32651747570.html?spm=a2g0s.9042311.0.0.34684c4dUx5EOb)  
[2.8" TFT non touch version](https://www.aliexpress.com/item/33012793224.html?spm=a2g0s.9042311.0.0.34684c4dUx5EOb)  
[BME280 sensor](https://www.aliexpress.com/item/32849462236.html?spm=a2g0s.9042311.0.0.34684c4dUx5EOb)

Uses the [OpenWeather](https://openweathermap.org/api) API for current conditions and daily forecast. I have access to the daily forecast call. Newly created OpenWeather accounts may not as it's now a paid feature. If there is any interest I can update the code to read the forecast from the available free API call.

Most station settings are set through it's webserver. Current sensor readings can be sent to a ThingSpeak channnel.

3D printed case currently in development.

[Weather icons](https://www.deviantart.com/umutavci/art/weather-icon-set-165476034) from umutavci.

Password protected OTA updates enabled. Set the MD5 hash of your prefered password in Settings.h. To upload via IP in PlatformIO create a file called uploadPassword.ini in the same folder as platformio.ini with the following contents:

    [password]
    upload_password = xxxxx
    
Replace xxxxx with the plain text version of your OTA password.
