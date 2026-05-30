#ifndef ERENDERSETTINGS_H
#define ERENDERSETTINGS_H

#include <string>
#include <vector>

struct eLightingQuality {
    int fSubdivision;
    std::string fName;
};

class eRenderSettings {
public:
    static const std::vector<eLightingQuality>
    sLightingQualityOptions;
    static eLightingQuality sLightingQuality;

    static void write();
    static bool read();
};

#endif // ERENDERSETTINGS_H
