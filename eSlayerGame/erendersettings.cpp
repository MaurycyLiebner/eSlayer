#include "erendersettings.h"

#include "eloadtexthelper.h"

#include <eSlayerHelpers/egamedir.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <map>

const int eRenderSettings::sMinLightingQuality = 1;
const int eRenderSettings::sMaxLightingQuality = 5;
int eRenderSettings::sLightingQuality = sMaxLightingQuality;

void eRenderSettings::write() {
    const auto path = eGameDir::renderSettingsPath();
    std::ofstream file;
    file.open(path);
    file << "lightingQuality" << " \"" << sLightingQuality << "\"";
    file.close();
}

bool eRenderSettings::read() {
    const auto path = eGameDir::renderSettingsPath();
    const bool e = std::filesystem::exists(path);
    if(!e) return false;
    std::map<std::string, std::string> settings;
    const bool r = eLoadTextHelper::load(path, settings);
    if(!r) return false;
    const int lq = std::stoi(settings["lightingQuality"]);
    sLightingQuality = std::clamp(lq, sMinLightingQuality, sMaxLightingQuality);
    return true;
}
