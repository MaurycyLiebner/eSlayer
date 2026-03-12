#ifndef EUITEXTURES_H
#define EUITEXTURES_H

#include "../textures/etexture.h"

class eUITextures {
public:        
    static void sLoad(SDL_Renderer * const r);

    static std::shared_ptr<eTexture> sOrb;
    static std::shared_ptr<eTexture> sOrbFront;
private:
    static bool sLoaded;
};


#endif // EUITEXTURES_H
