#include "echarstextures.h"

#include <eSlayerHelpers/efileloaderbase.h>
#include <eSlayerHelpers/eexceptions.h>

eCharsTextures eCharsTextures::sInstance;

eCharTextures& eCharsTextures::get(const std::string& name) {
    return sInstance.getImpl(name);
}

eCharTextures& eCharsTextures::get(const int id) {
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
        const auto jdata = eFileLoaderBase::parse(dir, "units/chars.json");
        chars = jdata.get<std::vector<std::string>>();
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/chars/chars.json");
    }

    mChars.reserve(chars.size());
    for(const auto& name : chars) {
        try {
            const int oldId = mChars.id(name);
            if(oldId != -1) {
                eExceptions::showDialog("Duplicate character '" + name + "' in " + dir + "/chars/chars.json");
                continue;
            }

            eCharTextures texs;
            const int id = mChars.nextId();
            texs.setCharDataId(id);
            auto jdata = eFileLoaderBase::parse(dir, "units/" + name + "/" + name + ".json");
            texs.load(jdata);
            mChars.add(name, texs);
        } catch(const std::exception& e) {
            eExceptions::showDialog(e);
        }
    }
}

eCharTextures& eCharsTextures::getImpl(const std::string &name) {
    const int id = mChars.id(name);
    return getImpl(id);
}

eCharTextures& eCharsTextures::getImpl(const int id) {
    if(id < 0 || id >= mChars.size()) {
        eRuntimeThrow("Index out of range.");
    }
    return mChars.get(id);
}
