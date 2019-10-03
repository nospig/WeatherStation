#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include "OctoPrintMonitor.h"

const int JOB_DECODE_SIZE   = 1024;   // TODO
const int PRINT_DECODE_SIZE = 2048;   // TODO


void OctoPrintMonitor::init(String server, int port, String apiKey, String userName, String password)
{
    updateSettings(server, port, apiKey, userName, password);
}

void OctoPrintMonitor::updateSettings(String server, int port, String apiKey, String userName, String password)
{
    this->apiKey = apiKey;
    this->server = server;
    this->userName = userName;
    this->password = password;
    this->port = port;
}

void OctoPrintMonitor::update()
{
    updateJobStatus();
    updatePrinterStatus();
}

void OctoPrintMonitor::updateJobStatus()
{
    String result;
    int httpCode;
    
    httpCode = performAPIGet(OCTOPRINT_JOB, result);
    
    if(httpCode == 200)
    {
        data.validJobData = true;
        deserialiseJob(result);

        Serial.println(result);
    }
    else
    {
        data.validJobData = false;
    }
}

void OctoPrintMonitor::updatePrinterStatus()
{
    String result;
    int httpCode;

    httpCode = performAPIGet(OCTOPRINT_PRINTER, result);
    
     if(httpCode == 200)
    {
        data.validPrintData = true;
        deserialisePrint(result);

        Serial.println(result);
    }
    else
    {
        data.validPrintData = false;
    }
}

int OctoPrintMonitor::performAPIGet(String apiCall, String& payload)
{
    // must be in this order
    WiFiClient client;
    HTTPClient http;

    http.begin(client, this->server, this->port, apiCall);
    http.addHeader("X-Api-Key", this->apiKey);

    if(this->userName != "")
    {
        http.setAuthorization(this->userName.c_str(), this->password.c_str());
    }

    int httpCode = http.GET();

    Serial.print("HTTP CODE: ");
    Serial.println(httpCode);

    if (httpCode > 0)
    {
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
        {            
            payload = http.getString();
        }        
    }
    
    return httpCode;
}

void OctoPrintMonitor::deserialiseJob(String payload)
{
    DynamicJsonDocument doc(JOB_DECODE_SIZE);
    deserializeJson(doc, payload);

    data.jobState = (const char*)doc["state"];
}

void OctoPrintMonitor::deserialisePrint(String payload)
{
    DynamicJsonDocument doc(PRINT_DECODE_SIZE);
    deserializeJson(doc, payload);

    data.tool0Temp = doc["temperature"]["tool0"]["actual"];
    data.tool0Target= doc["temperature"]["tool0"]["target"];

    data.bedTemp = doc["temperature"]["bed"]["actual"];
    data.bedTarget = doc["temperature"]["bed"]["target"];
}

