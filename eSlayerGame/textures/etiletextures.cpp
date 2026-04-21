#include "etiletextures.h"

#include "espriteloader.h"
#include "../efileloader.h"

eTileTextures::eTileTextures() {}

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
    eSpriteLoader loader("Textures", mDirName + "/" + mName, res, r);
    mColl = std::make_shared<eTextureCollection>();
    loader.loadAll(*mColl);
}

void eTileTextures::loadFixedSize(const int w, const int h,
                                  const eResolution& res,
                                  SDL_Renderer* const r) {
    if(mLoaded) return;
    mLoaded = true;
    const auto suffix = res.textureSuffix();
    const auto dir = "Textures";
    const auto path = mDirName + "/" + mName + suffix + "_0.png";
    const auto atlas = eFileLoader::readTexture(r, dir, path);
    mColl = std::make_shared<eTextureCollection>();
    const int aw = atlas->width();
    const int ah = atlas->height();
    for(int x = 0; x < aw; x += w) {
        for(int y = 0; y < ah; y += h) {
            const auto tex = std::make_shared<eTexture>();
            tex->setParentTexture(SDL_Rect{x, y, w, h}, atlas);
            mColl->addTexture(tex);
        }
    }
}

void eTileTextures::initialize(
    const std::string& dirName,
    const std::string& name) {
    mDirName = dirName;
    mName = name;
}
