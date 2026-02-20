#ifndef EWINDOWSETTINGS_H
#define EWINDOWSETTINGS_H

#include "eresolution.h"

struct eWindowSettings {
    bool fFullscreen = false;
    eResolution fRes = eResolution(1280, 720);

    void write() const;
    void read();
};

#endif // EWINDOWSETTINGS_H
