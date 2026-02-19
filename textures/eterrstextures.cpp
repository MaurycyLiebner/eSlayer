#include "eterrstextures.h"

#include "../efileloader.h"

eTerrsTextures eTerrsTextures::sInstance;

eTerrsTextures::eTerrsTextures() {}

eTerrTextures* eTerrsTextures::get(const std::string& name) {
    return sInstance.getImpl(name);
}

void eTerrsTextures::load() {
    return sInstance.loadImpl();
}

void eTerrsTextures::loadImpl() {
    if(mLoaded) return;
    mLoaded = true;

    const auto dir = "Textures";

    const auto jdata = eFileLoader::parse(dir, "terrain/terrain.json");
    const auto terrs = jdata.get<std::vector<std::string>>();

    for(const auto& name : terrs) {
        auto& texs = mTerrs[name];
        texs.setName(name);
    }
}

eTerrTextures* eTerrsTextures::getImpl(const std::string& name) {
    const auto it = mTerrs.find(name);
    if(it == mTerrs.end()) return nullptr;
    return &it->second;
}
