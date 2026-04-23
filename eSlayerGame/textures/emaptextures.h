#ifndef EMAPTEXTURES_H
#define EMAPTEXTURES_H

#include "etexture.h"
#include "etexturecollection.h"

#include <eSlayerHelpers/estringidmapvector.h>

class eMapTextures {
public:
    static void load(const eResolution& res,
                     SDL_Renderer* const r);

    static eTextureCollection sWalls;
    static eStringIdMapVector<std::shared_ptr<eTexture>> sTexs;
private:
    static bool sLoaded;
};

#endif // EMAPTEXTURES_H
