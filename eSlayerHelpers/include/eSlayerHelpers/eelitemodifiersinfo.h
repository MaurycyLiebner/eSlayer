#ifndef EELITEMODIFIERSINFO_H
#define EELITEMODIFIERSINFO_H

#include "eslayerhelpersexport.h"

#include "estringidmapvector.h"
#include "eskills.h"

struct eEliteInfo {
    eSkillLevelsStats fMinions;
    eSkillLevelsStats fBoss;
};

class ESLAYERHELPERS_API eEliteModifiersInfo {
public:
    static void load();

    static eStringIdMapVector<eEliteInfo> sElite;
private:
    static bool sLoaded;
};

#endif // EELITEMODIFIERSINFO_H
