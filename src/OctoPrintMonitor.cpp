#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include "OctoPrintMonitor.h"

void OctoPrintMonitor::init(String server, int port, String apiKey, String userName, String password)
{
    updateSettings(server, port, apiKey, userName, password);

    update();
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
    String result = performAPIGet(OCTOPRINT_JOB);
    
    if(result != "")
    {
        Serial.println(result);
    }
}

void OctoPrintMonitor::updatePrinterStatus()
{
    String result = performAPIGet(OCTOPRINT_PRINTER);
    
    if(result != "")
    {
        Serial.println(result);
    }
}

String OctoPrintMonitor::performAPIGet(String apiCall)
{
    // must be in this order
    WiFiClient client;
    HTTPClient http;

    http.begin(client, this->server, this->port, apiCall);
    http.addHeader("X-Api-Key", this->apiKey);
    http.setAuthorization(this->userName.c_str(), this->password.c_str());      // TODO if set in settings

    int httpCode = http.GET();

    //Serial.print("HTTP CODE: ");
    //Serial.println(httpCode);

    if (httpCode > 0)
    {
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
        {            
            return http.getString();
        }        
    }
    return "";
}

