#ifndef EEFFECTSTEXTURES_H
#define EEFFECTSTEXTURES_H

#include "eeffecttextures.h"

#include <eSlayerHelpers/estringidmapvector.h>

class eEffectsTextures {
public:
    static void load();

    static eStringIdMapVector<eEffectTextures> sEffects;
private:
    static bool sLoaded;
};

#endif // EEFFECTSTEXTURES_H
