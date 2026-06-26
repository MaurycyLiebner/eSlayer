#ifndef EMAPSETTINGS_H
#define EMAPSETTINGS_H

#include "eslayerhelpersexport.h"

#include "estringidmapvector.h"

#include <cstdint>
#include <map>
#include <string>

enum class eConnectionType {
    plain, cave, portal, stairs, trapDoor
};

enum class eAreaType {
    camp, open, dungeon
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

struct eBlueprintCount {
    eBlueprintCount() {}
    eBlueprintCount(const uint16_t type,
                    const int count) :
        fType(type),
        fCount(count) {}

    uint16_t fType;
    int fCount;
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
    std::string fName;
    eAreaType fType;
    bool fWaypoint;
    uint8_t fTerrainType;
    eMapMonsterSettings fMonsters;
    std::vector<eObjectCount> fObjects;
    std::vector<eObjectCount> fOutsideObjects;
    std::vector<eBlueprintCount> fBlueprints;
    uint8_t fLightness = 180;
    uint8_t fContrast = 140;
    uint8_t fLevel = 0;
    uint16_t fSize = 0;
    std::map<std::string, eConnectionSettings> fConnections;
};

struct eMapSettings {
    eStringIdMapVector<eAreaSettings> fAreas;
    uint8_t fRespawnMap;
    int fMaxSize = 80;
    uint8_t fActId;
};

struct ESLAYERHELPERS_API eMapsSettings {
    static void load();

    static eStringIdMapVector<eMapSettings> sMaps;
private:
    static bool sLoaded;
};

#endif // EMAPSETTINGS_H
