#ifndef ESOUNDS_H
#define ESOUNDS_H

#include "esoundvector.h"

#include <eSlayerHelpers/estringidmapvector.h>

class eSounds {
public:
    static void load();

    static eStringIdMapVector<eSoundVector> sSounds;
private:
    static bool sLoaded;
};

#endif // ESOUNDS_H
