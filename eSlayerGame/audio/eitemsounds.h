#ifndef EITEMSOUNDS_H
#define EITEMSOUNDS_H

#include <eSlayerHelpers/estringidmapvector.h>

class eItemSounds {
public:
    static void load();

    static eStringIdMapVector<int> sSoundIds;
private:
    static bool sLoaded;
};

#endif // EITEMSOUNDS_H
