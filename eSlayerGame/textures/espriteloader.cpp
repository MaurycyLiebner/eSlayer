#include "espriteloader.h"

#include "etexturecollection.h"

#include "../efileloader.h"

#include <eSlayerHelpers/eexceptions.h>

eSpriteLoader::eSpriteLoader(const std::string& dir,
                             const std::string& path,
                             SDL_Renderer* const r,
                             const SDL_Color& colorKey) :
    mDir(dir), mPath(path),
    mRenderer(r), mColorKey(colorKey) {

}

std::shared_ptr<eTexture> eSpriteLoader::load(const int i) {
    initialize();
    if(mSpriteCoords.size() <= i) {
        eExceptions::logError(
            "Texture " + std::to_string(i) + " out of range " +
            mDir + "/" + mPath + ".");
        return nullptr;
    }
    const auto tex = std::make_shared<eTexture>();
    const auto& off = mSpriteOffsets[i];
    tex->setOffset(off.x, off.y);
    tex->setParentTexture(mSpriteCoords[i], mAtlas);
    return tex;
}

std::shared_ptr<eTexture> eSpriteLoader::load(const int i,
                                              eTextureCollection &coll) {
    const auto tex = load(i);
    if(!tex) return nullptr;
    coll.addTexture(tex);
    return tex;
}

int eSpriteLoader::loadAll(eTextureCollection& coll) {
    initialize();
    const int n = mSpriteCoords.size();
    for(int i = 0; i < n; i++) {
        load(i, coll);
    }
    return n;
}

void eSpriteLoader::initialize() {
    if(mInitialized) return;
    mInitialized = true;

    mAtlas = eFileLoader::readTexture(mRenderer, mDir, mPath + ".png", mColorKey);

    const auto csvPath = mPath + ".csv";
    const auto doc = eFileLoader::readCsv(mDir, csvPath);
    const int nrows = doc.GetRowCount();
    mSpriteCoords.reserve(nrows);
    for(int i = 0; i < nrows; i++) {
        auto& rect = mSpriteCoords.emplace_back();
        auto& offset = mSpriteOffsets.emplace_back();
        const auto row = doc.GetRow<int>(i);
        if(row.size() != 6) {
            eExceptions::logError(
                "Invalid atlas rect/offset at line " +
                std::to_string(i + 1) + " in " +
                csvPath + ".");
            continue;
        }
        rect.x = row[0];
        rect.y = row[1];
        rect.w = row[2];
        rect.h = row[3];

        offset.x = row[4];
        offset.y = row[5];
    }
}
