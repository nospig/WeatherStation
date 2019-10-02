#ifndef _octoPrintMonitor_h
#define _octoPrintMonitor_h

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SettingsManager.h"

class OctoPrintMonitor
{
    public:
        void init(SettingsManager* settingsManager);

    private:
        SettingsManager* settingsManager;   
};

#endif // _octoPrintMonitor_h