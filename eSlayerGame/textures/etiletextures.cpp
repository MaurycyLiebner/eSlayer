#include "etiletextures.h"

#include "espriteloader.h"
#include "../efileloader.h"
#include "../eresolution.h"

const std::shared_ptr<eTexture>& eTileTextures::getTexture(const int id) const {
    return mColl->getTexture(id);
}

int eTileTextures::size() const {
    return mColl->size();
}

void eTileTextures::load(const eResolution& res,
                         SDL_Renderer* const r) {
    if(mLoaded) return;
    mLoaded = true;
    eSpriteLoader loader("Textures", "terrain/" + fName + "/" + fName, res, r);
    mColl = std::make_shared<eTextureCollection>();
    mColl->addTexture(nullptr);
    loader.loadAll(*mColl);
}
