#ifndef EUITEXTURES_H
#define EUITEXTURES_H

#include "../textures/etexturecollection.h"

class eUITextures {
public:        
    static void sLoad(SDL_Renderer * const r);

    static eTextureCollection sOrb;
private:
    static bool sLoaded;
};


#endif // EUITEXTURES_H
