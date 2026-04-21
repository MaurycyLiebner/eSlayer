#ifndef EMAPSETTINGS_H
#define EMAPSETTINGS_H

#include <cstdint>
#include <set>
#include <map>
#include <string>

struct eMapSettings {
    std::set<uint16_t> fMonsters;
    uint8_t fLightness = 180;
    uint8_t fContrast = 140;

    static void load();

    static std::map<std::string, eMapSettings> sMaps;
private:
    static bool sLoaded;
};

#endif // EMAPSETTINGS_H
