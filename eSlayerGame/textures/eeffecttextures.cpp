#include "eeffecttextures.h"

#include "espriteloader.h"

eEffectTextures::eEffectTextures() {}

const std::shared_ptr<eTexture>& eEffectTextures::getTexture(const int id) const {
    return mColl->getTexture(id);
}

int eEffectTextures::size() const {
    return mColl->size();
}

void eEffectTextures::load(const eResolution& res,
                           SDL_Renderer* const r) {
    if(mLoaded) return;
    mLoaded = true;
    const auto suffix = res.textureSuffix();
    const auto path = "effects/" + mName + suffix;
    eSpriteLoader loader("Textures", path, r);
    mColl = std::make_shared<eTextureCollection>();
    loader.loadAll(*mColl);
}
