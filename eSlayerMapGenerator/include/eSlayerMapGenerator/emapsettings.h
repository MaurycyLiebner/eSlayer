#ifndef EMAPSETTINGS_H
#define EMAPSETTINGS_H

#include "eslayermapgeneratorexport.h"

#include <eSlayerHelpers/estringidmapvector.h>

#include <cstdint>
#include <map>
#include <string>

enum class eConnectionType {
    plain, cave, portal, stairs
};

enum class eAreaType {
    open, dungeon
};

struct eTypeCount {
    eTypeCount() {}
    eTypeCount(const uint16_t type,
               const int count) :
        fType(type),
        fCount(count) {}

    uint16_t fType;
    int fCount;
};

struct eMonsterCount : public eTypeCount {
    int fGroupSize;
    bool fElite;
};

struct eMapMonsterSettings {
    std::vector<eMonsterCount> fTypes;
};

enum class eConnectionDir : uint8_t {
    up, down
};

struct eConnectionSettings {
    eConnectionType fType;
    eConnectionDir fDir;
    std::string fMap;
};

struct eAreaSettings {
    eAreaType fType;
    uint8_t fTerrainType;
    eMapMonsterSettings fMonsters;
    std::vector<eTypeCount> fObjects;
    std::vector<eTypeCount> fOutsideObjects;
    uint8_t fLightness = 180;
    uint8_t fContrast = 140;
    uint8_t fLevel = 0;
    std::map<std::string, eConnectionSettings> fConnections;
};

struct eMapSettings {
    eStringIdMapVector<eAreaSettings> fAreas;
};

struct ESLAYERMAPGENERATOR_API eMapsSettings {
    static void load();

    static eStringIdMapVector<eMapSettings> sMaps;
private:
    static bool sLoaded;
};

#endif // EMAPSETTINGS_H
