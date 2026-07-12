#ifndef EBLUEPRINTS_H
#define EBLUEPRINTS_H

#include "estringidmapvector.h"

#include <cstdint>
#include <vector>
#include <optional>

struct eBlueprintObject {
    uint8_t fType;
    std::optional<uint8_t> fSubtype;
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
};

class ESLAYERHELPERS_API eBlueprints {
public:
    static eStringIdMapVector<eBlueprint> sBlueprints;

    static void load();
private:
    static bool sLoaded;
};

#endif // EBLUEPRINTS_H
