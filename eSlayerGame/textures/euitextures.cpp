#include "euitextures.h"

#include "espriteloader.h"

eTextureCollection eUITextures::sOrb;
bool eUITextures::sLoaded = false;

void eUITextures::sLoad(SDL_Renderer* const r) {
    if(sLoaded) return;
    sLoaded = true;
    eSpriteLoader loader("Textures", "ui/orb", r);
    loader.load(0, sOrb);
    loader.load(1, sOrb);
}
