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
    mercenary,
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
    std::map<int, std::vector<int>> fItemTypes;
    std::map<int, std::vector<int>> fPotionTypes;
    std::map<int, std::vector<int>> fMercTypes;

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
