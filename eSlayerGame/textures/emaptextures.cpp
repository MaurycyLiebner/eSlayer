#include "emaptextures.h"

#include "../efileloader.h"

eStringIdMapVector<std::shared_ptr<eTexture>>
eMapTextures::sTexs;
bool eMapTextures::sLoaded = false;

void eMapTextures::load(const eResolution& res,
                        SDL_Renderer* const r) {
    if(sLoaded) return;
    sLoaded = true;
    const auto suffix = res.textureSuffix();
    const auto dir = "Textures";
    const auto path = "maps/tile" + suffix + ".png";
    const auto tile = eFileLoader::readTexture(r, dir, path);
    sTexs.add("tile", tile);
}
