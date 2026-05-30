#include "ewindowsettings.h"

#include "eloadtexthelper.h"
#include "elanguage.h"

#include <eSlayerHelpers/egamedir.h>

#include <filesystem>
#include <fstream>
#include <map>

void eWindowSettings::write() const {
    const auto path = eGameDir::windowSettingsPath();
    std::ofstream file;
    file.open(path);
    file << "fullscreen" << " " <<
        (fFullscreen ? "\"true\"" : "\"false\"") << "\n";
    const auto wStr = std::to_string(fRes.width());
    file << "width" << " " << "\"" << wStr << "\"" << "\n";
    const auto hStr = std::to_string(fRes.height());
    file << "height" << " " << "\"" << hStr << "\"" << "\n";
    file << "language" << " " << "\"" << fLanguage.fName << "\"";
    file.close();
}

bool eWindowSettings::read() {
    const auto path = eGameDir::windowSettingsPath();
    const bool e = std::filesystem::exists(path);
    if(!e) return false;
    std::map<std::string, std::string> settings;
    const bool r = eLoadTextHelper::load(path, settings);
    if(!r) return false;
    fFullscreen = settings["fullscreen"] == "true";
    const auto widthStr = settings["width"];
    const auto heightStr = settings["height"];
    if(!widthStr.empty() && !heightStr.empty()) {
        const int width = std::stoi(widthStr);
        const int height = std::stoi(heightStr);
        fRes = eResolution(width, height);
    }
    const auto languageStr = settings["language"];
    eLanguage::setLanguage(languageStr);
    fLanguage = eLanguage::sLanguage;
    return true;
}
