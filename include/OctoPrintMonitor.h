#ifndef _octoPrintMonitor_h
#define _octoPrintMonitor_h

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define OCTOPRINT_JOB       "/api/job"
#define OCTOPRINT_PRINTER   "/api/printer?exclude=sd"

typedef struct PrinterMonitorData
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

    bool valid;
} PrinterMonitorData;

class OctoPrintMonitor
{
    public:
        void init(String apiKey, String server, String userName, String password);
        void updateSettings(String apiKey, String server, String userName, String password);
        void update();
        PrinterMonitorData* getCurrentData() { return &data; }

    private:
        void updateJobStatus();
        void updatePrinterStatus();
        String performAPIGet(String apiCall);

        String apiKey;
        String server;
        String userName;
        String password;
        PrinterMonitorData data;
};

#endif // _octoPrintMonitor_h