#include "echarstextures.h"

#include "../efileloader.h"

eCharsTextures eCharsTextures::sInstance;

eCharsTextures::eCharsTextures() {}

eCharTextures *eCharsTextures::get(const std::string& name) {
    return sInstance.getImpl(name);
}

void eCharsTextures::load() {
    return sInstance.loadImpl();
}

void eCharsTextures::loadImpl() {
    if(mLoaded) return;
    mLoaded = true;

    const auto dir = "Textures";

    const auto jdata = eFileLoader::parse(dir, "chars/chars.json");
    const auto chars = jdata.get<std::vector<std::string>>();

    for(const auto& name : chars) {
        auto& texs = mChars[name];
        texs.setName(name);
        auto jdata = eFileLoader::parse(dir, "chars/" + name + "/" + name + ".json");
        texs.load(jdata);
    }
}

eCharTextures* eCharsTextures::getImpl(const std::string &name) {
    const auto it = mChars.find(name);
    if(it == mChars.end()) return nullptr;
    return &it->second;
}
