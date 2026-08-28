#include "esoundoptions.h"

#include "eloadtexthelper.h"

#include <eSlayerHelpers/egamedir.h>

#include <filesystem>
#include <algorithm>
#include <fstream>

int eSoundOptions::sMusicVolume = 100;
int eSoundOptions::sSoundVolume = 100;
int eSoundOptions::sEffectsVolume = 100;

void eSoundOptions::write() {
    const auto path = eGameDir::soundSettingsPath();
    std::ofstream file;
    file.open(path);
    file << "musicVolume \"" << sMusicVolume << "\"" << std::endl;
    file << "soundVolume \"" << sSoundVolume << "\"" << std::endl;;
    file << "effectsVolume \"" << sEffectsVolume << "\"";
    file.close();
}

bool eSoundOptions::read() {
    const auto path = eGameDir::soundSettingsPath();
    const bool e = std::filesystem::exists(path);
    if(!e) return false;
    std::map<std::string, std::string> settings;
    const bool r = eLoadTextHelper::load(path, settings);
    if(!r) return false;

    const auto get = [&](const std::string& name) {
        const auto str = settings[name];
        try {
            return std::clamp(std::stoi(str), 0, 100);
        } catch(...) {
            return 100;
        }
    };

    sMusicVolume = get("musicVolume");
    sSoundVolume = get("soundVolume");
    sEffectsVolume = get("effectsVolume");

    return true;
}
