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

struct eObjectCount {
    eObjectCount() {}
    eObjectCount(const uint16_t type,
                const int count,
                 const int minArea) :
        fType(type),
        fCount(count),
        fMinArea(minArea) {}

    uint16_t fType;
    int fCount;
    int fMinArea;
};

struct eMonsterCount {
    uint16_t fBaseType;
    std::vector<uint16_t> fTypes;
    std::vector<uint16_t> fBossTypes;
    int fCount;
    int fMinArea;
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
    std::vector<eObjectCount> fObjects;
    std::vector<eObjectCount> fOutsideObjects;
    uint8_t fLightness = 180;
    uint8_t fContrast = 140;
    uint8_t fLevel = 0;
    uint16_t fSize = 0;
    std::map<std::string, eConnectionSettings> fConnections;
};

struct eMapSettings {
    eStringIdMapVector<eAreaSettings> fAreas;
    int fMaxSize = 80;
};

struct ESLAYERMAPGENERATOR_API eMapsSettings {
    static void load();

    static eStringIdMapVector<eMapSettings> sMaps;
private:
    static bool sLoaded;
};

#endif // EMAPSETTINGS_H
