#ifndef EMAPSETTINGS_H
#define EMAPSETTINGS_H

#include <eSlayerHelpers/estringidmapvector.h>

#include <cstdint>
#include <set>
#include <map>
#include <string>

enum class eConnectionType {
    plain, cave, portal
};

enum class eAreaType {
    open, dungeon
};

struct eAreaSettings {
    eAreaType fType;
    std::set<uint16_t> fMonsters;
    uint8_t fLightness = 180;
    uint8_t fContrast = 140;
    std::map<std::string, eConnectionType> fConnections;
};

struct eMapSettings {
    eStringIdMapVector<eAreaSettings> fAreas;

    static void load();

    static std::map<std::string, eMapSettings> sMaps;
private:
    static bool sLoaded;
};

#endif // EMAPSETTINGS_H
