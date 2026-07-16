#ifndef EMAPSETTINGS_H
#define EMAPSETTINGS_H

#include "eslayerhelpersexport.h"

#include "estringidmapvector.h"

#include <eSlayerHelpers/eeffectsettings.h>

#include <cstdint>
#include <map>
#include <string>
#include <optional>

enum class eConnectionType {
    plain, cave, portal, stairs, trapDoor
};

enum class eAreaType {
    camp, open, dungeon
};

struct eObjectCount {
    eObjectCount() {}
    eObjectCount(const uint16_t type,
                 const std::optional<uint8_t> subtype,
                 const int count,
                 const int minArea) :
        fType(type),
        fSubtype(subtype),
        fCount(count),
        fMinArea(minArea) {}

    uint16_t fType;
    std::optional<uint8_t> fSubtype;
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

struct eAreaMonsterSettings {
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
    bool fWaypoint = false;
    uint8_t fTerrainType;
    eAreaMonsterSettings fMonsters;
    std::vector<eObjectCount> fObjects;
    std::vector<eObjectCount> fOutsideObjects;
    std::vector<eBlueprintCount> fBlueprints;
    uint8_t fLightness = 180;
    uint8_t fContrast = 140;
    uint8_t fLevel = 1;
    uint16_t fSize = 50;

    uint8_t fRoomSize = 6;
    uint8_t fConnThick = 2;
    uint8_t fConnLen = 4;

    bool fMerge = true;

    std::vector<eEffectSettings> fEffects;
    std::map<std::string, eConnectionSettings> fConnections;
};

struct eDifficultyMapSettings {
    std::map<int, eAreaSettings> fDiffs;
};

struct eMapSettings {
    eStringIdMapVector<eDifficultyMapSettings> fAreas;
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
