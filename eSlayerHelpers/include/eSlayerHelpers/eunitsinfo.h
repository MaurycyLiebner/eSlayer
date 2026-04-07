#ifndef EUNITSINFO_H
#define EUNITSINFO_H

#include "eslayerhelpersexport.h"

#include "estringidmapvector.h"
#include "eunitinfo.h"

class ESLAYERHELPERS_API eUnitsInfo {
public:
    static void load();

    static eStringIdMapVector<eUnitInfo> sUnits;
private:
    static bool sLoaded;
};

#endif // EUNITSINFO_H
