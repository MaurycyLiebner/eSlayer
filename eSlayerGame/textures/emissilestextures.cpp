#include "emissilestextures.h"

#include "../efileloader.h"
#include "espriteloader.h"

#include <eSlayerHelpers/eskills.h>
#include <eSlayerHelpers/eitemsdata.h>

eStringIdMapVector<eMissileTextures>
eMissilesTextures::sMissiles;
bool eMissilesTextures::sLoaded = false;

const std::shared_ptr<eTexture>&
eMissileAnimTextures::get(
    const int dir, const int frame) {
    return mDirs[dir].getTexture(frame);
}

void eMissileAnimTextures::load(
    const eResolution& res,
    SDL_Renderer* const r,
    const eMissileAnim& info) {
    const auto dir = "Textures";
    eSpriteLoader loader(dir, info.fPath, res, r);
    mDirs.reserve(info.fNDirs);
    for(int d = 0; d < info.fNDirs; d++) {
        auto& coll = mDirs.emplace_back();
        for(int f = 0; f < info.fNFrames; f++) {
            loader.load(d*info.fNFrames + f, coll);
        }
    }
}

const std::shared_ptr<eTexture>&
eMissileTextures::get(const int animId,
                      const int dir,
                      const int frame) {
    auto& anim = mAnims.get(animId);
    return anim.get(dir, frame);
}

void eMissileTextures::load(const eResolution& res,
                            SDL_Renderer* const r,
                            const eMissileInfo& info) {
    for(const auto& it : info.mAnims) {
        const auto& name = it.fName;
        const auto& ainfo = it.fValue;
        eMissileAnimTextures texs;
        texs.load(res, r, ainfo);
        mAnims.add(name, texs);
    }
}

void eMissilesTextures::load(const eResolution& res,
                             SDL_Renderer* const r) {
    if(sLoaded) return;
    sLoaded = true;
    for(const auto& m : eMissilesInfo::sMissiles) {
        const auto& name = m.fName;
        const auto& info = m.fValue;
        eMissileTextures texs;
        texs.load(res, r, info);
        sMissiles.add(name, texs);
    }
}
