#ifndef EOBJECTSINFO_H
#define EOBJECTSINFO_H

#include "eslayerhelpersexport.h"

#include "estringidmapvector.h"

enum class eObjectType {
    none,
    treasure,
    waypoint,
    portal
};

struct eObjectInfo {
    float fSize;
    eObjectType fType;
    std::string fTexStr;
    int fTexId;
    bool fObstacle;
    bool fWalkable;
};

class ESLAYERHELPERS_API eObjectsInfo {
public:
    static void load();

    static eStringIdMapVector<eObjectInfo> sObjects;
private:
    static bool sLoaded;
};

#endif // EOBJECTSINFO_H
