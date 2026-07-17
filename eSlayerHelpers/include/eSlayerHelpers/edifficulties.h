#ifndef EDIFFICULTIES_H
#define EDIFFICULTIES_H

#include "estringidmapvector.h"

struct eDifficulty {
    float fResistPenalty = 0.f;
    float fLeechPenalty = 0.f;
    float fColdLengthPenalty = 0.f;
    float fFreezeLengthPenalty = 0.f;
};

class ESLAYERHELPERS_API eDifficulties {
public:
    static eStringIdMapVector<eDifficulty> sDifficulties;
    static int sDifficulty;

    static void load();
private:
    static bool sLoaded;
};

#endif // EDIFFICULTIES_H
