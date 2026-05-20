#ifndef EOBJECTSINFO_H
#define EOBJECTSINFO_H

#include "eslayerhelpersexport.h"

#include "estringidmapvector.h"

enum class eObjectType {
    none,
    treasure
};

struct eObjectInfo {
    float fSize;
    eObjectType fType;
    std::string fTexStr;
    int fTexId;
    bool fObstacle;
};

class ESLAYERHELPERS_API eObjectsInfo {
public:
    static void load();

    static eStringIdMapVector<eObjectInfo> sObjects;
private:
    static bool sLoaded;
};

#endif // EOBJECTSINFO_H
