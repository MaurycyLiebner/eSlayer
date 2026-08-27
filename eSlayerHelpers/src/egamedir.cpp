#include "eSlayerHelpers/egamedir.h"

std::string eGameDir::sExePath;

std::string eGameDir::path(const std::string& path) {
    return sExePath + "../../" + path;
}

std::string eGameDir::windowSettingsPath() {
    return path("windowSettings.txt");
}

std::string eGameDir::renderSettingsPath() {
    return path("videoSettings.txt");
}

std::string eGameDir::soundSettingsPath() {
    return path("soundSettings.txt");
}
