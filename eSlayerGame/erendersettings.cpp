#include "erendersettings.h"

#include "eloadtexthelper.h"

#include <eSlayerHelpers/egamedir.h>

#include <filesystem>
#include <fstream>
#include <map>

bool eRenderSettings::sRenderObjectShadows = true;
bool eRenderSettings::sRenderWallShadows = true;

void eRenderSettings::write() {
    const auto path = eGameDir::renderSettingsPath();
    std::ofstream file;
    file.open(path);
    file << "renderObjectShadows" << " " <<
        (sRenderObjectShadows ? "\"true\"" : "\"false\"") << "\n";
    file << "renderWallShadows" << " " <<
        (sRenderWallShadows ? "\"true\"" : "\"false\"");

    file.close();
}

bool eRenderSettings::read() {
    const auto path = eGameDir::renderSettingsPath();
    const bool e = std::filesystem::exists(path);
    if(!e) return false;
    std::map<std::string, std::string> settings;
    const bool r = eLoadTextHelper::load(path, settings);
    if(!r) return false;
    sRenderObjectShadows = settings["renderObjectShadows"] == "true";
    sRenderWallShadows = settings["renderWallShadows"] == "true";
    return true;
}
