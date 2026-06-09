#ifndef EWINDOWSETTINGS_H
#define EWINDOWSETTINGS_H

#include "eresolution.h"
#include "elanguage.h"

struct eWindowSettings {
    bool fFullscreen = false;
    eResolution fRes = eResolution(1280, 720);
    eLanguage fLanguage = eLanguage::sLanguage;
    int fThreads = -1;

    void write() const;
    bool read();
};

#endif // EWINDOWSETTINGS_H
