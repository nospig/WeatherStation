#include "MQTTManager.h"
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

AsyncMqttClient MQTTManager::mqttClient;
bool MQTTManager::connected = false;
Ticker MQTTManager::reconnectTimer;
SettingsManager* MQTTManager::settingsManager;
void (* MQTTManager::subscribeCallback)(const char *, const char *);


void MQTTManager::reconnect()
{
    // call after settings change
    mqttClient.disconnect();
    setBrokerDetails();
    connectToMqtt();
}

void MQTTManager::setBrokerDetails()
{
    strcpy(mqttBroker, settingsManager->getMqttBroker().c_str());
    strcpy(mqttUsername, settingsManager->getMqttUsername().c_str());
    strcpy(mqttPassword, settingsManager->getMqttPassword().c_str());
    strcpy(mqttClientID, settingsManager->getMqttClientId().c_str());

    if(strlen(mqttClientID))
    {
        mqttClient.setClientId(mqttClientID);
    }
    if(strlen(mqttUsername) && strlen(mqttPassword))
    {
        mqttClient.setCredentials(mqttUsername, mqttPassword);
    }
    mqttClient.setServer(mqttBroker, settingsManager->getMqttPort());
}

void MQTTManager::updateSensorReadings(float temp, float humidity, float pressure)
{
    char buffer[32];

    if(!connected || !settingsManager->getMqttEnabled())
    {
        //Serial.println("NOT sending MQTT");
        return;
    }

    sprintf(buffer, "%f", temp);
    mqttClient.publish(settingsManager->getMqttTempTopic().c_str(), 0, false, buffer);
    
    sprintf(buffer, "%f", humidity);
    mqttClient.publish(settingsManager->getMqttHumidityTopic().c_str(), 0, false, buffer);
    
    sprintf(buffer, "%f", pressure);
    mqttClient.publish(settingsManager->getMqttPressureTopic().c_str(), 0, false, buffer);
}

void MQTTManager::init(SettingsManager* settingsManager)
{
    connected = false;
    subscribeCallback = nullptr;
    this->settingsManager = settingsManager;

    setBrokerDetails();
  
    mqttClient.onConnect(onConnect);
    mqttClient.onDisconnect(onDisconnect);
    mqttClient.onMessage(onMessage);

    connectToMqtt();
}

void MQTTManager::connectToMqtt()
{
    if(settingsManager->getMqttEnabled() && settingsManager->getMqttBroker() != "")
    {
        //Serial.println("Connecting to MQTT");
        mqttClient.connect();
    }
}

void MQTTManager::onConnect(bool sessionPresent)
{
    connected = true;

    if(settingsManager->getMqttDisplayTopic() != "")
    {
        mqttClient.subscribe(settingsManager->getMqttDisplayTopic().c_str(), 0);
    }
}

void MQTTManager::onDisconnect(AsyncMqttClientDisconnectReason reason)
{
    //char buffer[32];

    //sprintf(buffer, "MQTT Disconnected: %d\n", (int)reason);
    //Serial.println(buffer);

    connected = false;

    if(WiFi.isConnected())
    {
        reconnectTimer.once(MQTT_RECONNECT_TIME, connectToMqtt);
    }
}

void MQTTManager::onMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
    if(subscribeCallback != nullptr)
    {
        char payloadBuffer[len+1];
        strncpy(payloadBuffer, payload, len);
        payloadBuffer[len] = '\0';

        subscribeCallback(topic, payloadBuffer);
    }
}

void MQTTManager::setSubscribeCallback(void (* callback)(const char *, const char *))
{
    subscribeCallback = callback;
}
