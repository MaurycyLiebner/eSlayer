#include "egamedir.h"

#include <SDL3/SDL_filesystem.h>

std::string eGameDir::path(const std::string& path) {
    return exeDir() + "../" + path;
}

std::string eGameDir::windowSettingsPath() {
    return path("windowSettings.txt");
}

std::string eGameDir::exeDir() {
    const auto d = SDL_GetBasePath();
    const std::string str(d);
    return str;
}
