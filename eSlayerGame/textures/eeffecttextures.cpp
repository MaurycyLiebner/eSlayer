#include "eeffecttextures.h"

#include "espriteloader.h"

eEffectTextures::eEffectTextures() {}

const std::shared_ptr<eTexture>& eEffectTextures::getTexture(const int id) const {
    return mColl->getTexture(id);
}

int eEffectTextures::size() const {
    return mColl->size();
}

void eEffectTextures::load(SDL_Renderer* const r) {
    if(mLoaded) return;
    mLoaded = true;
    eSpriteLoader loader("Textures", "effects/" + mName,
                         r, SDL_Color{0, 0, 0, 0});
    mColl = std::make_shared<eTextureCollection>();
    loader.loadAll(*mColl);
}
