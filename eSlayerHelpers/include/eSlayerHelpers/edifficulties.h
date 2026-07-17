#ifndef EDIFFICULTIES_H
#define EDIFFICULTIES_H

#include "estringidmapvector.h"

class ESLAYERHELPERS_API eDifficulties {
public:
    static eStringIdMapVector<int> sDifficulties;
    static int sDifficulty;

    static void load();
private:
    static bool sLoaded;
};

#endif // EDIFFICULTIES_H
