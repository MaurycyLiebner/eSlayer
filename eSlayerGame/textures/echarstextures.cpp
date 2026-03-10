#include "echarstextures.h"

#include "../efileloader.h"

#include <eSlayerHelpers/eexceptions.h>

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

    std::vector<std::string> chars;
    try {
        const auto jdata = eFileLoader::parse(dir, "chars/chars.json");
        chars = jdata.get<std::vector<std::string>>();
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/chars/chars.json");
    }

    for(const auto& name : chars) {
        try {
            const auto it = mChars.find(name);
            if(it != mChars.end()) {
                eExceptions::showDialog("Duplicate character '" + name + "' in " + dir + "/chars/chars.json");
                continue;
            }
            auto& texs = mChars[name];
            texs.setName(name);
            auto jdata = eFileLoader::parse(dir, "chars/" + name + "/" + name + ".json");
            texs.load(jdata);
        } catch(const std::exception& e) {
            eExceptions::showDialog(e);
            mChars.erase(name);
        }
    }
}

eCharTextures* eCharsTextures::getImpl(const std::string &name) {
    const auto it = mChars.find(name);
    if(it == mChars.end()) return nullptr;
    return &it->second;
}
