#include "emissilestextures.h"

#include "../efileloader.h"
#include "espriteloader.h"

eStringIdMapVector<eMissileTextures>
eMissilesTextures::sMissiles;
bool eMissilesTextures::sDataLoaded = false;
bool eMissilesTextures::sTexsLoaded = false;

const std::shared_ptr<eTexture>&
eMissileAnim::get(const int dir, const int frame) {
    return fDirs[dir].getTexture(frame);
}

void eMissileAnim::load(SDL_Renderer* const r) {
    const auto dir = "Textures";
    eSpriteLoader loader(dir, fPath, r);
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

void eMissileTextures::load(SDL_Renderer* const r) {
    for(const auto& anim : mAnims) {
        anim.fValue.load(r);
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

void eMissilesTextures::loadData() {
    if(sDataLoaded) return;
    sDataLoaded = true;

    const std::string dir = "Textures";

    const auto jdata = eFileLoader::parse(dir, "missiles/missiles.json");
    const auto missiles = jdata.get<std::vector<std::string>>();

    sMissiles.reserve(missiles.size());
    for(const auto& name : missiles) {
        const auto pathBase = "missiles/" + name;
        const auto jdata = eFileLoader::parse(dir, "missiles/" + name + ".json");
        const int dirs = jdata["directions"];
        const float lighting = jdata.value("lighting", 0);
        const auto& anims = jdata["animations"];
        eMissileTextures texs;
        texs.setLighting(lighting);
        for(auto& [aname, animData] : anims.items()) {
            eMissileAnim anim;
            const int nFrames = animData.value("frames", 0);
            anim.fNFrames = nFrames;
            anim.fNDirs = dirs;
            anim.fPath = pathBase + "_" + aname;
            texs.mAnims.add(aname, anim);
        }
        sMissiles.add(name, texs);
    }
}

void eMissilesTextures::loadTextures(SDL_Renderer* const r) {
    if(sTexsLoaded) return;
    sTexsLoaded = true;
    for(const auto& m : sMissiles) {
        m.fValue.load(r);
    }
}
