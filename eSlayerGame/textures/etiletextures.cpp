#include "etiletextures.h"

#include "espriteloader.h"

eTileTextures::eTileTextures() {}

const std::shared_ptr<eTexture>& eTileTextures::getTexture(const int id) const {
    return mColl->getTexture(id);
}

int eTileTextures::size() const {
    return mColl->size();
}

void eTileTextures::load(SDL_Renderer* const r) {
    if(mLoaded) return;
    mLoaded = true;
    eSpriteLoader loader("Textures", mDirName + "/" + mName,
                         r, SDL_Color{172, 172, 172, 255});
    mColl = std::make_shared<eTextureCollection>(r);
    loader.loadAll(*mColl);
}

void eTileTextures::initialize(
    const std::string& dirName,
    const std::string& name) {
    mDirName = dirName;
    mName = name;
}
