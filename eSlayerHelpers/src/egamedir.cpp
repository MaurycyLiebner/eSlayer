#include "eSlayerHelpers/egamedir.h"

std::string eGameDir::sExePath;

std::string eGameDir::path(const std::string& path) {
    return sExePath + "../../" + path;
}

std::string eGameDir::windowSettingsPath() {
    return path("windowSettings.txt");
}
