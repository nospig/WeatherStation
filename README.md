# Weather Station

Another version of the popular ESP2866 weather station.

**Features**

* Creates an access point for connecting to any WiFi network
* BME280 sensor for local temperature, pressure and humidity readings
* 2.8" colour TFT for display
* Uses a cheap Wemos D1 as the controller
* 3 different display modes. All display modes show WiFi strength, clock with date and time and use colourful icons
  * Summary - indoor conditions, current weather and 3 day forecast
  * 5 day forecast
  * Detailed current weather with indoor conditions
* All settings controlled through a web interface which also displays sensor readings and weather conditions
* Uses OpenWeather API for current weather and forecasts
* Can report sensor readings to ThingSpeak
* Can publish sensor readings to a MQTT broker
* Can perform screengrabs and send them to a Processing sketch over serial connection
* Password protected OTA updates enabled through PlatformIO (not through web interface)

**API**

Uses the [OpenWeather](https://openweathermap.org/api) API for current conditions and daily forecast. I have access to the daily forecast call. Newly created OpenWeather accounts may not as it's now a paid feature. If there is any interest I can update the code to read the forecast from the available free API call.

**Libraries**

* Adafruit BME280
* ArduinoJson
* ESPAsyncWiFiManager
* ESPAsyncTCP
* ESPAsyncWebServer
* TaskScheduler
* TFT_eSPI
* ThingSpeak
* AsyncMqttClient
* NTPClient

Big thanks to all library developers who make developing projects such as this quite simple.

**Hardware**

Built using the following hardware from AliExpress.

[Wemos D1 mini ESP8266 board](https://www.aliexpress.com/item/32651747570.html?spm=a2g0s.9042311.0.0.34684c4dUx5EOb)  
[2.8" TFT non touch version](https://www.aliexpress.com/item/33012793224.html?spm=a2g0s.9042311.0.0.34684c4dUx5EOb)  
[BME280 sensor](https://www.aliexpress.com/item/32849462236.html?spm=a2g0s.9042311.0.0.34684c4dUx5EOb)

Also works fine with a NodeMCU board.

**Wiring**

Mini RST -> TFT RST  
Mini D0  -> TFT LED  
Mini D5  -> TFT SCK  
Mini D6  -> TFT MISO (only required if screen grab feature required)  
Mini D7  -> TFT MOSI  
Mini D1  -> BME SCL  
Mini D2  -> BME SDA  
Mini D3  -> TFT DC  

Splice some wires and connect VCC on the TFT and BME to the Mini 3V3 pin. Do the same for GND to the Mini GND pin.

**Note**

There is a setting in the TFT_eSPI library that has to be set the same as you wire the TFT to the Mini.

**OTA updates**

Password protected OTA updates enabled. Set the MD5 hash of your prefered password in Settings.h. To upload via IP in PlatformIO create a file called uploadPassword.ini in the same folder as platformio.ini with the following contents:

    [password]
    upload_password = xxxxx
    
Replace xxxxx with the plain text version of your OTA password.

**MQTT**

The station can publish sensor readings to a MQTT broker. Enter broker details on the MQTT settings page through the webserver. Choose a topic root to identify your station, i.e. bedroom, livingRoom. Sensor details will be sent using the topic root and '/sensors'. i.e. bedroom/sensors  

Payload is a json file in the following format. `{"temp":"32.7","humidity":"28","pressure":"1012"}`

The station will subscribe to messages using the topic root and '/display', i.e. bedroom/display. Send payload of 'off' to switch the display off, 'on' to switch display on. Use lower case for display and on/off.

**Case**

3D printed case currently in development.

[Weather icons](https://www.deviantart.com/umutavci/art/weather-icon-set-165476034) from umutavci.
