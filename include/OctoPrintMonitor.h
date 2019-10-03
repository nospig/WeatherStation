#ifndef _octoPrintMonitor_h
#define _octoPrintMonitor_h

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define OCTOPRINT_JOB       "/api/job"
#define OCTOPRINT_PRINTER   "/api/printer?exclude=sd"

typedef struct OctoPrintMonitorData
{
    // job
    String fileName;
    unsigned int estimatedPrintTime;
    unsigned int filamentLength;
    String jobState;
    float percentComplete;
    unsigned int printTimeElapsed;
    unsigned int printTimeRemaining;

    // printer
    float tool0Temp;
    float tool0Target;
    float bedTemp;
    float bedTarget;
    String printState;

    bool validJobData;
    bool validPrintData;
} PrinterMonitorData;

class OctoPrintMonitor
{
    public:
        void init(String server, int port, String apiKey, String userName, String password);
        void updateSettings(String server, int port, String apiKey, String userName, String password);
        void update();
        OctoPrintMonitorData* getCurrentData() { return &data; }

    private:
        void updateJobStatus();
        void updatePrinterStatus();
        int performAPIGet(String apiCall, String& payload);
        void deserialiseJob(String payload);
        void deserialisePrint(String payload);

        String apiKey;
        String server;
        String userName;
        String password;
        int port;

        OctoPrintMonitorData data;
};

#endif // _octoPrintMonitor_h