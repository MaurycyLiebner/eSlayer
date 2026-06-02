#ifndef EELITEMODIFIERSINFO_H
#define EELITEMODIFIERSINFO_H

#include "eslayerhelpersexport.h"

#include "estringidmapvector.h"
#include "emodscollection.h"
#include "ecolor.h"

struct eEliteInfo {
    eModsCollectionLevels fMinions;
    eModsCollectionLevels fBoss;
    eColor fBossColorMod{1.f, 1.f, 1.f, 1.f};
};

class ESLAYERHELPERS_API eEliteModifiersInfo {
public:
    static void load();

    static eStringIdMapVector<eEliteInfo> sElite;
    static const int sMaxUnitLevel;
private:
    static bool sLoaded;
};

#endif // EELITEMODIFIERSINFO_H
