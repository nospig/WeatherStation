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
    strcpy(mqqtBroker, settingsManager->getMQTTBroker().c_str());
    strcpy(mqqtUsername, settingsManager->getMQTTUsername().c_str());
    strcpy(mqqtPassword, settingsManager->getMQTTPassword().c_str());

    mqttClient.setServer(mqqtBroker, settingsManager->getMQTTPort());
    mqttClient.setCredentials(mqqtUsername, mqqtPassword);
}

void MQTTManager::updateSensorReadings(float temp, float humidity, float pressure)
{
    String output;
    char buffer[32];
    DynamicJsonDocument jsonDoc(128);

    if(!connected || !settingsManager->getMQTTEnabled())
    {
        Serial.println("NOT sending MQTT");
        return;
    }

    sprintf(buffer, "%.1f", temp);
    jsonDoc["temp"] = buffer;
    
    sprintf(buffer, "%.0f", humidity);
    jsonDoc["humidity"] = buffer;
    
    sprintf(buffer, "%.0f", pressure);
    jsonDoc["pressure"] = buffer;

    serializeJson(jsonDoc, output);

    String topic = settingsManager->getMQTTTopic() + "/sensors";
    mqttClient.publish(topic.c_str(), 0, false, output.c_str());
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
    if(settingsManager->getMQTTEnabled() && settingsManager->getMQTTBroker() != "" && settingsManager->getMQTTUsername() != "" && settingsManager->getMQTTPassword() != "")
    {
        //Serial.println("Connecting to MQTT");
        mqttClient.connect();
    }
}

void MQTTManager::onConnect(bool sessionPresent)
{
    connected = true;

    String topic = settingsManager->getMQTTTopic() + "/" + MQTT_DISPLAY_SUBTOPIC;
    mqttClient.subscribe(topic.c_str(), 0);
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

        // TODO, reduce String usage
        String subTopic = String(topic);
        subTopic.remove(0, settingsManager->getMQTTTopic().length() + 1); // remove topic and / 
        subscribeCallback(subTopic.c_str(), payloadBuffer);
    }
}

void MQTTManager::setSubscribeCallback(void (* callback)(const char *, const char *))
{
    subscribeCallback = callback;
}
