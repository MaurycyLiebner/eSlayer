#include "esoundoptions.h"

#include "eloadtexthelper.h"

#include <eSlayerHelpers/egamedir.h>

#include <filesystem>
#include <algorithm>
#include <fstream>

int eSoundOptions::sMusicVolume = 100;
int eSoundOptions::sSoundVolume = 100;

void eSoundOptions::write() {
    const auto path = eGameDir::soundSettingsPath();
    std::ofstream file;
    file.open(path);
    file << "musicVolume" << " \"" << sMusicVolume << "\"" << std::endl;
    file << "soundVolume" << " \"" << sSoundVolume << "\"";
    file.close();
}

bool eSoundOptions::read() {
    const auto path = eGameDir::soundSettingsPath();
    const bool e = std::filesystem::exists(path);
    if(!e) return false;
    std::map<std::string, std::string> settings;
    const bool r = eLoadTextHelper::load(path, settings);
    if(!r) return false;
    const auto musicVolumeStr = settings["musicVolume"];
    const auto soundVolumeStr = settings["soundVolume"];
    sMusicVolume = std::clamp(std::stoi(musicVolumeStr), 0, 100);
    sSoundVolume = std::clamp(std::stoi(soundVolumeStr), 0, 100);
    return true;
}
