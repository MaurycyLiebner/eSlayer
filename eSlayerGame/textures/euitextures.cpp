#include "euitextures.h"

#include "espriteloader.h"

std::shared_ptr<eTexture> eUITextures::sOrb;
std::shared_ptr<eTexture> eUITextures::sOrbFront;
bool eUITextures::sLoaded = false;

void eUITextures::sLoad(SDL_Renderer* const r) {
    if(sLoaded) return;
    sLoaded = true;
    eSpriteLoader loader("Textures", "ui/orb", r);
    sOrb = loader.load(0);
    sOrbFront = loader.load(1);
}
