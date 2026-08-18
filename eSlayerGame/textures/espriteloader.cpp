#include "espriteloader.h"

#include "etexturecollection.h"

#include "../efileloader.h"
#include "../eresolution.h"

#include <eSlayerHelpers/eexceptions.h>

eSpriteLoader::eSpriteLoader(const std::string& dir,
                             const std::string& path,
                             const eResolution& res,
                             SDL_Renderer* const r,
                             const SDL_Color& colorKey,
                             const int maxRows) :
    mDir(dir), mPath(path), mRes(res),
    mRenderer(r), mColorKey(colorKey),
    mMaxRows(maxRows) {}

std::shared_ptr<eTexture> eSpriteLoader::load(const int i) {
    initialize();
    if(mSprites.size() <= i) {
        eRuntimeThrow("Texture " + std::to_string(i) +
                      " out of range " +
                      mDir + "/" + mPath + ".");
    }
    auto& row = mSprites[i];
    const int atlasId = row.fAtlasId;
    const auto& rect = row.fCoords;
    if(atlasId < 0) return nullptr;
    const auto tex = std::make_shared<eTexture>();
    const auto& off = row.fOffset;
    tex->setOffset(off.x, off.y);
    const auto& atlas = mAtlases[atlasId];
    const auto& atex = atlas->requestTex(mRenderer);
    tex->setAtlas(row.fCoords, atex);
    return tex;
}

std::shared_ptr<eTexture> eSpriteLoader::load(
    const int i, eTextureCollection &coll) {
    const auto tex = load(i);
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

    int nrows = doc.GetRowCount();
    if(mMaxRows > 0) {
        nrows = std::min(nrows, mMaxRows);
    }
    mSprites.reserve(nrows);
    for(int i = 0; i < nrows; i++) {
        auto& sprite = mSprites.emplace_back();
        auto& atlasId = sprite.fAtlasId;
        auto& rect = sprite.fCoords;
        auto& offset = sprite.fOffset;
        const auto row = doc.GetRow<int>(i);
        if(row.size() != 7) {
            eRuntimeThrow("Invalid atlas rect/offset at line " +
                          std::to_string(i + 1) + " in " +
                          csvPath + ".");
        }

        atlasId = row[0];

        rect.x = row[1];
        rect.y = row[2];
        rect.w = row[3];
        rect.h = row[4];

        offset.x = row[5];
        offset.y = row[6];

        if(atlasId < 0) {
        } else if(atlasId > mAtlases.size()) {
            eRuntimeThrow("Atlases not in order in \"" + mDir + "/" + csvPath + "\".");
        } else if(atlasId == mAtlases.size()) {
            const auto idStr = "_" + std::to_string(atlasId);
            const auto atlas = std::make_shared<eAtlas>(mColorKey);
            const auto path = mPath + suffix + idStr + ".png";
            atlas->loadSurf(mDir, path);
            mAtlases.emplace_back(atlas);
        }
    }
}
