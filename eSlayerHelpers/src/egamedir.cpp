#include "eSlayerHelpers/egamedir.h"

std::string eGameDir::path(const std::string& path) {
    return "../../" + path;
}

std::string eGameDir::windowSettingsPath() {
    return path("windowSettings.txt");
}
