#include "enovastextures.h"

#include "../efileloader.h"
#include "espriteloader.h"

#include <eSlayerHelpers/eskills.h>

eStringIdMapVector<eNovaTextures>
eNovasTextures::sNovas;
bool eNovasTextures::sDataLoaded = false;
bool eNovasTextures::sTexsLoaded = false;

const std::shared_ptr<eTexture>&
eNovaTextures::get(const int frame) {
    return mTexs.getTexture(frame);
}

void eNovaTextures::load(const eResolution& res,
                         SDL_Renderer* const r) {
    const auto dir = "Textures";
    eSpriteLoader loader(dir, mPath, res, r);
    loader.loadAll(mTexs);
}

int eNovaTextures::nFrames() const {
    return mTexs.size();
}

void eNovasTextures::loadData() {
    if(sDataLoaded) return;
    sDataLoaded = true;

    const std::string dir = "Textures";

    const auto jdata = eFileLoader::parse(dir, "novas/novas.json");
    const auto novas = jdata.get<std::vector<std::string>>();

    sNovas.reserve(novas.size());
    for(const auto& name : novas) {
        const auto pathBase = "novas/" + name;
        const auto jdata = eFileLoader::parse(dir, "novas/" + name + ".json");
        const float lighting = jdata.value("lighting", 0.f);
        eNovaTextures texs;
        texs.setLighting(lighting);
        texs.mPath = pathBase;
        sNovas.add(name, texs);
    }

    for(const auto& it : eSkills::sSkills) {
        auto& skill = it.fValue;
        skill.fNovaId = sNovas.id(skill.fNovaStr);
    }
}

void eNovasTextures::loadTextures(const eResolution& res,
                                  SDL_Renderer* const r) {
    if(sTexsLoaded) return;
    sTexsLoaded = true;
    for(const auto& n : sNovas) {
        n.fValue.load(res, r);
    }
}
