#include "eeffectstextures.h"

#include "../efileloader.h"

#include <eSlayerHelpers/eexceptions.h>

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

    std::vector<std::string> effs;
    try {
        const auto jdata = eFileLoader::parse(dir, "effects/effects.json");
        effs = jdata.get<std::vector<std::string>>();
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/effects/effects.json");
    }

    for(const auto& name : effs) {
        try {
            const auto it = mEffects.find(name);
            if(it != mEffects.end()) {
                eExceptions::showDialog("Duplicate effect '" + name + "' in " + dir + "/effects/effects.json");
                continue;
            }
            auto& texs = mEffects[name];
            texs.setName(name);
        } catch(const std::exception& e) {
            eExceptions::showDialog(e);
            mEffects.erase(name);
        }
    }
}

eEffectTextures* eEffectsTextures::getImpl(const std::string& name) {
    const auto it = mEffects.find(name);
    if(it == mEffects.end()) return nullptr;
    return &it->second;
}
