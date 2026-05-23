#include "emissilestextures.h"

#include "../efileloader.h"
#include "espriteloader.h"

#include <eSlayerHelpers/eskills.h>
#include <eSlayerHelpers/eitemsdata.h>

eStringIdMapVector<eMissileTextures>
eMissilesTextures::sMissiles;
int eMissilesTextures::sFleshId = -1;
int eMissilesTextures::sIceId = -1;

bool eMissilesTextures::sDataLoaded = false;
bool eMissilesTextures::sTexsLoaded = false;

const std::shared_ptr<eTexture>&
eMissileAnim::get(const int dir, const int frame) {
    return fDirs[dir].getTexture(frame);
}

void eMissileAnim::load(const eResolution& res,
                        SDL_Renderer* const r) {
    const auto dir = "Textures";
    eSpriteLoader loader(dir, fPath, res, r);
    fDirs.reserve(fNDirs);
    for(int d = 0; d < fNDirs; d++) {
        auto& coll = fDirs.emplace_back();
        for(int f = 0; f < fNFrames; f++) {
            loader.load(d*fNFrames + f, coll);
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
                            SDL_Renderer* const r) {
    for(const auto& anim : mAnims) {
        anim.fValue.load(res, r);
    }
}

int eMissileTextures::nFrames(const int animId) const {
    const auto& anim = mAnims.get(animId);
    return anim.fNFrames;
}

int eMissileTextures::nDirs(const int animId) const {
    const auto& anim = mAnims.get(animId);
    return anim.fNDirs;
}

int eMissileTextures::animId(const std::string& name) const {
    return mAnims.id(name);
}

void eMissilesTextures::loadData() {
    if(sDataLoaded) return;
    sDataLoaded = true;

    const std::string dir = "Textures";

    const auto jdata = eFileLoader::parse(dir, "missiles/missiles.json");
    const auto missiles = jdata.get<std::vector<std::string>>();

    sMissiles.reserve(missiles.size() + 1);
    sMissiles.add("none", eMissileTextures());
    for(const auto& name : missiles) {
        const auto pathBase = "missiles/" + name + "/" + name;
        const auto jdata = eFileLoader::parse(dir, pathBase + ".json");
        const int dirs = jdata["directions"];
        const float lighting = jdata.value("lighting", 0.f);
        const auto typeStr = jdata.value("type", "regular");
        eMissileType type;
        if(typeStr == "explosion") {
            type = eMissileType::explosion;
        } else if(typeStr == "regular") {
            type = eMissileType::regular;
        } else {
            eRuntimeThrow("Unknown missile type \"" + typeStr + "\" in " +
                          dir + "/" + pathBase + ".json");
        }
        const auto& anims = jdata["animations"];
        eMissileTextures texs;
        texs.mLighting = lighting;
        texs.mType = type;
        for(auto& [aname, animData] : anims.items()) {
            eMissileAnim anim;
            const int nFrames = animData.value("frames", 0);
            anim.fNFrames = nFrames;
            anim.fNDirs = dirs;
            anim.fPath = pathBase + "_" + aname;
            texs.mAnims.add(aname, anim);
        }
        texs.mAppearAnimId = texs.animId("appear");
        texs.mBaseAnimId = texs.animId("base");
        texs.mHitAnimId = texs.animId("hit");

        const int id = sMissiles.add(name, texs);
        if(name == "flesh") {
            sFleshId = id;
        } else if(name == "ice") {
            sIceId = id;
        }
    }

    for(const auto& it : eSkills::sSkills) {
        auto& skill = it.fValue;
        skill.fMissileId = sMissiles.id(skill.fMissileStr);
    }

    for(const auto& it : eItemsData::sItems) {
        auto& item = it.fValue;
        item.fMissileId = sMissiles.id(item.fMissileStr);
    }
}

void eMissilesTextures::loadTextures(const eResolution& res,
                                     SDL_Renderer* const r) {
    if(sTexsLoaded) return;
    sTexsLoaded = true;
    for(const auto& m : sMissiles) {
        m.fValue.load(res, r);
    }
}
