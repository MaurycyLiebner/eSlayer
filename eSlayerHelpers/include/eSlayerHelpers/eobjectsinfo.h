#ifndef EOBJECTSINFO_H
#define EOBJECTSINFO_H

#include "eslayerhelpersexport.h"

#include "estringidmapvector.h"

#include "eitemdrop.h"

enum class eObjectType {
    none,
    treasure,
    waypoint,
    portal,
    stash,
    trapDoor,
    portalDoor,
    portalArea,
    spawnArea,
    message
};

enum class eBlockLightType {
    none, center, rect
};

struct eObjectInfo {
    float fWidth;
    float fHeight;

    eBlockLightType fBlocksLight;
    bool fShadow;
    bool fFlat;
    bool fSplit;
    float fLightRadius;

    eObjectType fType;
    std::string fTexStr;
    int fTexId;
    bool fObstacle;
    bool fWalkable;

    // for treasures
    std::vector<eItemDrop> fItemDrops;

    int fTriggerSound = -1;
    int fAppearSound = -1;

    int fKey = -1;
};

class ESLAYERHELPERS_API eObjectsInfo {
public:
    static void load();

    static eStringIdMapVector<eObjectInfo> sObjects;
private:
    static bool sLoaded;
};

#endif // EOBJECTSINFO_H
