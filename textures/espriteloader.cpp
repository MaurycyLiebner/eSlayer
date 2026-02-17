#include "espriteloader.h"

#include "etexturecollection.h"

#include <rapidcsv.h>

eSpriteLoader::eSpriteLoader(const std::string& path,
                             SDL_Renderer* const r,
                             const bool colorKey) :
    mPath(path), mRenderer(r), mColorKey(colorKey) {

}

std::shared_ptr<eTexture> eSpriteLoader::load(const int i) {
    initialize();
    if(mSpriteCoords.size() <= i) {
        printf("Texture %i out of range %s.\n",
               i, mPath.c_str());
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

void eSpriteLoader::initialize() {
    if(mInitialized) return;
    mInitialized = true;

    mAtlas = std::make_shared<eTexture>();
    mAtlas->load(mRenderer, mPath + ".png", mColorKey);

    const auto csvPath = mPath + ".csv";
    rapidcsv::Document doc(csvPath, rapidcsv::LabelParams(-1, -1));
    const int nrows = doc.GetRowCount();
    mSpriteCoords.reserve(nrows);
    for(int i = 0; i < nrows; i++) {
        auto& rect = mSpriteCoords.emplace_back();
        auto& offset = mSpriteOffsets.emplace_back();
        const auto row = doc.GetRow<int>(i);
        if(row.size() != 6) {
            printf("Invalid atlas rect/offset at line %i in %s.\n",
                   i + 1, csvPath.c_str());
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
