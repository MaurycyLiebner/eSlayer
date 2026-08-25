#ifndef EMUSIC_H
#define EMUSIC_H

#include "emusicvector.h"

#include <eSlayerHelpers/estringidmapvector.h>

class eMusic {
public:
    static void load();

    static eStringIdMapVector<eMusicVector> sMusic;
private:
    static bool sLoaded;
};


#endif // EMUSIC_H
