#include "emaptextures.h"

#include "../efileloader.h"

eStringIdMapVector<std::shared_ptr<eTexture>>
eMapTextures::sTexs;
bool eMapTextures::sLoaded = false;

void eMapTextures::load(SDL_Renderer* const r) {
    if(sLoaded) return;
    sLoaded = true;
    const auto dir = "Textures";
    const auto path = "maps/reveal.png";
    const auto reveal = eFileLoader::readTexture(r, dir, path);
    sTexs.add("reveal", reveal);
}
