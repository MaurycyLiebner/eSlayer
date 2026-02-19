#include "eeffectstextures.h"

#include "../efileloader.h"

eEffectsTextures eEffectsTextures::sInstance;

eEffectsTextures::eEffectsTextures() {}

eEffectTextures* eEffectsTextures::get(const std::string& name) {
    return sInstance.getImpl(name);
}

void eEffectsTextures::load() {
    return sInstance.loadImpl();
}

void eEffectsTextures::loadImpl() {
    if(mLoaded) return;
    mLoaded = true;

    const auto dir = "Textures";

    const auto jdata = eFileLoader::parse(dir, "effects/effects.json");
    const auto terrs = jdata.get<std::vector<std::string>>();

    for(const auto& name : terrs) {
        auto& texs = mTerrs[name];
        texs.setName(name);
    }
}

eEffectTextures* eEffectsTextures::getImpl(const std::string& name) {
    const auto it = mTerrs.find(name);
    if(it == mTerrs.end()) return nullptr;
    return &it->second;
}
