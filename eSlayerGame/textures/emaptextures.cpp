#include "emaptextures.h"

#include "../efileloader.h"
#include "../textures/espriteloader.h"

eTextureCollection eMapTextures::sWalls;
eStringIdMapVector<std::shared_ptr<eTexture>>
eMapTextures::sTexs;
bool eMapTextures::sLoaded = false;

void eMapTextures::load(const eResolution& res,
                        SDL_Renderer* const r) {
    if(sLoaded) return;
    sLoaded = true;
    const auto suffix = res.textureSuffix();
    const auto dir = "Textures";
    const auto path = "maps/wall" + suffix + "_0.png";
    const auto atlas = eFileLoader::readTexture(r, dir, path);
    const int aw = atlas->width();
    const int ah = atlas->height();
    const int w = aw/17;
    for(int x = 0; x < aw; x += w) {
        const auto tex = std::make_shared<eTexture>();
        tex->setParentTexture(SDL_Rect{x, 0, w, ah}, atlas);
        sWalls.addTexture(tex);
    }
}
