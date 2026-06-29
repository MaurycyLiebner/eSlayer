#ifndef EOBJECTSINFO_H
#define EOBJECTSINFO_H

#include "eslayerhelpersexport.h"

#include "estringidmapvector.h"

enum class eObjectType {
    none,
    treasure,
    waypoint,
    portal,
    stash,
    healer,
    trader,
    trapDoor,
    portalDoor,
    portalArea,
    spawnArea
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
    std::vector<int> fItemTypes;
    std::vector<int> fPotionTypes;
};

class ESLAYERHELPERS_API eObjectsInfo {
public:
    static void load();

    static eStringIdMapVector<eObjectInfo> sObjects;
private:
    static bool sLoaded;
};

#endif // EOBJECTSINFO_H
