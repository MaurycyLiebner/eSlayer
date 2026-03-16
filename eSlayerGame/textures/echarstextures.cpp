#include "echarstextures.h"

#include "../efileloader.h"

#include <eSlayerHelpers/eexceptions.h>

eCharsTextures eCharsTextures::sInstance;

eCharsTextures::eCharsTextures() {}

eCharTextures* eCharsTextures::get(const std::string& name) {
    return sInstance.getImpl(name);
}

eCharTextures* eCharsTextures::get(const int id) {
    return sInstance.getImpl(id);
}

void eCharsTextures::load() {
    return sInstance.loadImpl();
}

void eCharsTextures::loadImpl() {
    if(mLoaded) return;
    mLoaded = true;

    const auto dir = "Textures";

    std::vector<std::string> chars;
    try {
        const auto jdata = eFileLoader::parse(dir, "chars/chars.json");
        chars = jdata.get<std::vector<std::string>>();
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/chars/chars.json");
    }

    for(const auto& name : chars) {
        try {
            const auto it = mCharIdMap.find(name);
            if(it != mCharIdMap.end()) {
                eExceptions::showDialog("Duplicate character '" + name + "' in " + dir + "/chars/chars.json");
                continue;
            }
            const int id = mChars.size();
            mCharIdMap[name] = id;
            auto& texs = mChars.emplace_back();
            texs.setTypeId(id);
            texs.setName(name);
            auto jdata = eFileLoader::parse(dir, "chars/" + name + "/" + name + ".json");
            texs.load(jdata);
        } catch(const std::exception& e) {
            eExceptions::showDialog(e);
            mCharIdMap.erase(name);
        }
    }
}

eCharTextures* eCharsTextures::getImpl(const std::string &name) {
    const auto it = mCharIdMap.find(name);
    if(it == mCharIdMap.end()) return nullptr;
    return getImpl(it->second);
}

eCharTextures* eCharsTextures::getImpl(const int id) {
    if(id < 0 || id >= mChars.size()) {
        eRuntimeThrow("Index out of range.");
    }
    return &mChars[id];
}
