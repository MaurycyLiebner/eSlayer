#include "ewindowsettings.h"

#include <fstream>
#include <map>

#include "egamedir.h"
#include "eloadtexthelper.h"

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
    file.close();
}

void eWindowSettings::read() {
    const auto path = eGameDir::windowSettingsPath();
    std::map<std::string, std::string> settings;
    const bool r = eLoadTextHelper::load(path, settings);
    if(!r) return;
    fFullscreen = settings["fullscreen"] == "true";
    const auto widthStr = settings["width"];
    const auto heightStr = settings["height"];
    if(!widthStr.empty() && !heightStr.empty()) {
        const int width = std::stoi(widthStr);
        const int height = std::stoi(heightStr);
        fRes = eResolution(width, height);
    }
}
