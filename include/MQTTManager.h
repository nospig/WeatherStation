#ifndef _mqtt_manager_h
#define _mqtt_manager_h

#include <AsyncMqttClient.h>
#include "SettingsManager.h"
#include <Ticker.h>

class MQTTManager
{
    public:
        void init(SettingsManager* settingsManager);
    
        void updateSensorReadings(float temp, float humidity, float pressure);
        void reconnect();
        
    private:
        static void onConnect(bool sessionPresent);
        static void onDisconnect(AsyncMqttClientDisconnectReason reason);
        static void connectToMqtt();

        void setBrokerDetails();

        static AsyncMqttClient mqttClient;
        static SettingsManager* settingsManager;    
        static Ticker reconnectTimer;
        static bool connected;

        // MQTT library requires static strings. Not sure on max lengths required, TODO
        // max length set in mqqSettings.html
        char mqqtBroker[33];
        char mqqtUsername[17];
        char mqqtPassword[17];
};

#endif // _mqtt_manager_h