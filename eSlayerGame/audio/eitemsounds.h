#ifndef EITEMSOUNDS_H
#define EITEMSOUNDS_H

#include <eSlayerHelpers/estringidmapvector.h>

struct eItemSound {
    int fHit = -1;
    int fMiss = -1;
};

class eItemSounds {
public:
    static void load();

    static eStringIdMapVector<eItemSound> sSoundIds;
private:
    static bool sLoaded;
};

#endif // EITEMSOUNDS_H
