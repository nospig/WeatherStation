#ifndef _WebServer_H_
#define _WebServer_H_

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

class WebServer
{
    public:
        void init();
        AsyncWebServer* getServer();

        void updateSensorReadings(float temp, float humidity, float pressure);
        
    private:
        static void onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);
};

#endif