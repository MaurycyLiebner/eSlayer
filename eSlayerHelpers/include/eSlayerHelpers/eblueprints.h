#ifndef EBLUEPRINTS_H
#define EBLUEPRINTS_H

#include "estringidmapvector.h"

#include <cstdint>
#include <vector>

struct eBlueprintObject {
    uint8_t fObjId;
    float fX;
    float fY;
};

struct eBlueprint {
    float fWidth;
    float fHeight;

    std::vector<eBlueprintObject> fObjects;
};

class eBlueprints {
public:
    static eStringIdMapVector<eBlueprint> sBlueprints;

    static void load();
private:
    static bool sLoaded;
};

#endif // EBLUEPRINTS_H
