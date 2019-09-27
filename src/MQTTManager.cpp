#include "MQTTManager.h"
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

#include "Secrets.h"    // TODO, so I can push to github without showing my settings for now

AsyncMqttClient MQTTManager::mqttClient;
bool MQTTManager::connected = false;
Ticker MQTTManager::reconnectTimer;

void MQTTManager::updateSensorReadings(float temp, float humidity, float pressure)
{
    String output;
    char buffer[32];
    DynamicJsonDocument jsonDoc(128);

    if(!connected)
    {
        return;
    }

    sprintf(buffer, "%.1f", temp);
    jsonDoc["temp"] = buffer;
    
    sprintf(buffer, "%.0f", humidity);
    jsonDoc["humidity"] = buffer;
    
    sprintf(buffer, "%.0f", pressure);
    jsonDoc["pressure"] = buffer;

    serializeJson(jsonDoc, output);

    mqttClient.publish("bedroom/sensors", 0, false, output.c_str());
}

void MQTTManager::init(SettingsManager* settingsManager)
{
    connected = false;

    mqttClient.setServer(MQTT_HOST, MQTT_PORT);
    mqttClient.setCredentials(MQTT_USER, MQTT_PASS);
    
    mqttClient.onConnect(onConnect);
    mqttClient.onDisconnect(onDisconnect);

    connectToMqtt();
}

void MQTTManager::connectToMqtt()
{
    mqttClient.connect();
}

void MQTTManager::onConnect(bool sessionPresent)
{
    connected = true;

    //Serial.println("Connected to MQTT, session present: ");
    //Serial.println(sessionPresent);
}

void MQTTManager::onDisconnect(AsyncMqttClientDisconnectReason reason)
{
    connected = false;

    if(WiFi.isConnected())
    {
        reconnectTimer.once(2, connectToMqtt);
    }
}