#ifndef EMAPTEXTURES_H
#define EMAPTEXTURES_H

#include "etexture.h"

#include <eSlayerHelpers/estringidmapvector.h>

class eMapTextures {
public:
    static void load(SDL_Renderer* const r);

    static eStringIdMapVector<std::shared_ptr<eTexture>> sTexs;
private:
    static bool sLoaded;
};

#endif // EMAPTEXTURES_H
