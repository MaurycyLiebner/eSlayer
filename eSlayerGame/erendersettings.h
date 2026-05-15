#ifndef ERENDERSETTINGS_H
#define ERENDERSETTINGS_H

class eRenderSettings {
public:
    static const int sMinLightingQuality;
    static const int sMaxLightingQuality;
    static int sLightingQuality;

    static void write();
    static bool read();
};

#endif // ERENDERSETTINGS_H
