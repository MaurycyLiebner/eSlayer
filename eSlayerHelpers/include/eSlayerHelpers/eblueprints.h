#ifndef EBLUEPRINTS_H
#define EBLUEPRINTS_H

#include "estringidmapvector.h"

#include "eitemdrop.h"

#include <cstdint>
#include <vector>
#include <optional>
#include <set>

struct eBlueprintObject {
    uint8_t fType;
    std::optional<uint8_t> fSubtype;

    // for units
    std::set<uint8_t> fElite;
    int fUnitCount;
    bool fSuperUnique;

    // for units and treasures
    std::vector<eItemDrop> fItemDrops;

    float fX;
    float fY;
    std::optional<float> fWidth;
    std::optional<float> fHeight;
};

struct eBlueprint {
    float fWidth;
    float fHeight;

    std::vector<eBlueprintObject> fObjects;
    std::vector<eBlueprintObject> fTerrain;
    std::vector<eBlueprintObject> fUnits;
};

class ESLAYERHELPERS_API eBlueprints {
public:
    static eStringIdMapVector<eBlueprint> sBlueprints;

    static void load();
private:
    static bool sLoaded;
};

#endif // EBLUEPRINTS_H
