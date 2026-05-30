#include "erendersettings.h"

#include "eloadtexthelper.h"

#include <eSlayerHelpers/egamedir.h>

#include <filesystem>
#include <fstream>
#include <map>

const std::vector<eLightingQuality>
eRenderSettings::sLightingQualityOptions {
    {1, "low"}, {3, "medium"}, {5, "high"}
};
eLightingQuality eRenderSettings::sLightingQuality = sLightingQualityOptions.back();

void eRenderSettings::write() {
    const auto path = eGameDir::renderSettingsPath();
    std::ofstream file;
    file.open(path);
    file << "lightingQuality" << " \"" << sLightingQuality.fName << "\"";
    file.close();
}

bool eRenderSettings::read() {
    const auto path = eGameDir::renderSettingsPath();
    const bool e = std::filesystem::exists(path);
    if(!e) return false;
    std::map<std::string, std::string> settings;
    const bool r = eLoadTextHelper::load(path, settings);
    if(!r) return false;
    const auto qName = settings["lightingQuality"];
    for(const auto& o : sLightingQualityOptions) {
        if(o.fName != qName) continue;
        sLightingQuality = o;
        break;
    }
    return true;
}
