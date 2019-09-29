#ifndef _Mqtt_manager_h
#define _Mqtt_manager_h

#include <AsyncMqttClient.h>
#include "SettingsManager.h"
#include <Ticker.h>

#define MQTT_RECONNECT_TIME 5
#define MQTT_DISPLAY_SUBTOPIC "display"

class MQTTManager
{
    public:
        void init(SettingsManager* settingsManager);
    
        void updateSensorReadings(float temp, float humidity, float pressure);
        void reconnect();
        
        void setSubscribeCallback(void(* callback)(const char *, const char *));

    private:
        static void onConnect(bool sessionPresent);
        static void onDisconnect(AsyncMqttClientDisconnectReason reason);
        static void onMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);

        static void connectToMqtt();

        void setBrokerDetails();

        static AsyncMqttClient mqttClient;
        static SettingsManager* settingsManager;    
        static Ticker reconnectTimer;
        static bool connected;

        // Mqtt library requires static strings. Not sure on max lengths required, TODO
        // max length set in mqqSettings.html
        char mqttBroker[33];
        char mqttUsername[65];
        char mqttPassword[65];
        char mqttClientID[65];

        static void (* subscribeCallback)(const char *, const char *);
};

#endif // _Mqtt_manager_h