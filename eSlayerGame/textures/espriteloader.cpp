#include "espriteloader.h"

#include "etexturecollection.h"

#include "../efileloader.h"

#include <eSlayerHelpers/eexceptions.h>

eSpriteLoader::eSpriteLoader(const std::string& dir,
                             const std::string& path,
                             const eResolution& res,
                             SDL_Renderer* const r,
                             const SDL_Color& colorKey) :
    mDir(dir), mPath(path), mRes(res),
    mRenderer(r), mColorKey(colorKey) {}

std::shared_ptr<eTexture> eSpriteLoader::load(const int i) {
    initialize();
    if(mSprites.size() <= i) {
        eExceptions::logError(
            "Texture " + std::to_string(i) + " out of range " +
            mDir + "/" + mPath + ".");
        return nullptr;
    }
    const auto tex = std::make_shared<eTexture>();
    auto& row = mSprites[i];
    const auto& off = row.fOffset;
    tex->setOffset(off.x, off.y);
    const int atlasId = row.fAtlasId;
    const auto& atlas = mAtlases[atlasId];
    tex->setParentTexture(row.fCoords, atlas);
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
    const int n = mSprites.size();
    for(int i = 0; i < n; i++) {
        load(i, coll);
    }
    return n;
}

void eSpriteLoader::initialize() {
    if(mInitialized) return;
    mInitialized = true;

    const auto suffix = mRes.textureSuffix();
    const auto csvPath = mPath + suffix + ".csv";
    Document doc;
    try {
        doc = eFileLoader::readCsv(mDir, csvPath);
    } catch(...) {
        eRuntimeThrow("Failed to read \"" + mDir + "/" + csvPath + "\".");
    }

    const int nrows = doc.GetRowCount();
    mSprites.reserve(nrows);
    for(int i = 0; i < nrows; i++) {
        auto& sprite = mSprites.emplace_back();
        auto& atlasId = sprite.fAtlasId;
        auto& rect = sprite.fCoords;
        auto& offset = sprite.fOffset;
        const auto row = doc.GetRow<int>(i);
        if(row.size() != 7) {
            eExceptions::logError(
                "Invalid atlas rect/offset at line " +
                std::to_string(i + 1) + " in " +
                csvPath + ".");
            atlasId = 0;

            rect.x = 0;
            rect.y = 0;
            rect.w = 0;
            rect.h = 0;

            offset.x = 0;
            offset.y = 0;
            continue;
        }

        atlasId = row[0];

        if(atlasId < 0) {
            eRuntimeThrow("Atlas id less than 0 in \"" + mDir + "/" + csvPath + "\".");
        } else if(atlasId > mAtlases.size()) {
            eRuntimeThrow("Atlases not in order in \"" + mDir + "/" + csvPath + "\".");
        } else if(atlasId == mAtlases.size()) {
            const auto idStr = "_" + std::to_string(atlasId);
            const auto atlas = eFileLoader::readTexture(mRenderer, mDir, mPath + suffix + idStr + ".png", mColorKey);
            mAtlases.emplace_back(atlas);
        }

        rect.x = row[1];
        rect.y = row[2];
        rect.w = row[3];
        rect.h = row[4];

        offset.x = row[5];
        offset.y = row[6];
    }
}
