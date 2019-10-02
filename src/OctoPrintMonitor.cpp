#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include "OctoPrintMonitor.h"

void OctoPrintMonitor::init(String apiKey, String server, String userName, String password)
{
    updateSettings(apiKey, server, userName, password);

    update();
}

void OctoPrintMonitor::updateSettings(String apiKey, String server, String userName, String password)
{
    this->apiKey = apiKey;
    this->server = server;
    this->userName = userName;
    this->password = password;
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

    String url = this->server + apiCall;

    //Serial.println("Testing OctoPrintMonitor");
    //Serial.println("URL :" + url);

    http.begin(client, url);
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

