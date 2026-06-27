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
    spawnArea,
    empty
};

struct eObjectInfo {
    float fSize;
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
