#ifndef _mqtt_manager_h
#define _mqtt_manager_h

#include <AsyncMqttClient.h>
#include "SettingsManager.h"

class MQTTManager
{
    public:
        void init(SettingsManager* settingsManager);
    
        void updateSensorReadings(float temp, float humidity, float pressure);

    private:
        static void onConnect(bool sessionPresent);
        static void onDisconnect(AsyncMqttClientDisconnectReason reason);
        

        static AsyncMqttClient mqttClient;
        static SettingsManager* settingsManager;    
        static bool connected;
};

#endif // _mqtt_manager_h