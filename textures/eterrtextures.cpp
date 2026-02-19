#include "eterrtextures.h"

#include "espriteloader.h"

eTerrTextures::eTerrTextures() {}

const std::shared_ptr<eTexture>& eTerrTextures::getTexture(const int id) const {
    return mColl->getTexture(id);
}

int eTerrTextures::size() const {
    return mColl->size();
}

void eTerrTextures::load(SDL_Renderer* const r) {
    if(mLoaded) return;
    mLoaded = true;
    eSpriteLoader loader("Textures", "terrain/" + mName,
                         r, SDL_Color{172, 172, 172, 255});
    mColl = std::make_shared<eTextureCollection>(r);
    loader.loadAll(*mColl);
}
