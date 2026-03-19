#include "eserverchardata.h"

#include "eserverfileloader.h"

#include <eSlayerHelpers/eexceptions.h>

eServerCharData eServerCharData::sInstance;

eServerCharData::eServerCharData() {}

eCharData *eServerCharData::get(const std::string& name) {
    return sInstance.getImpl(name);
}

eCharData* eServerCharData::get(const int id) {
    return sInstance.getImpl(id);
}

void eServerCharData::load() {
    return sInstance.loadImpl();
}

void eServerCharData::loadImpl() {
    if(mLoaded) return;
    mLoaded = true;

    const auto dir = "Textures";

    const auto jdata = eServerFileLoader::parse(dir, "chars/chars.json");
    const auto chars = jdata.get<std::vector<std::string>>();

    mChars.reserve(chars.size());
    for(const auto& name : chars) {
        try {
            const int oldId = mChars.id(name);
            if(oldId != -1) {
                eExceptions::showDialog("Duplicate character '" + name + "' in " + dir + "/chars/chars.json");
                continue;
            }

            eCharData texs;
            const int id = mChars.nextId();
            texs.setTypeId(id);
            texs.setName(name);
            auto jdata = eServerFileLoader::parse(dir, "chars/" + name + "/" + name + ".json");
            texs.load(jdata);
            mChars.add(name, texs);
        } catch(const std::exception& e) {
            eExceptions::showDialog(e);
        }
    }
}

eCharData* eServerCharData::getImpl(const std::string &name) {
    const int id = mChars.id(name);
    return getImpl(id);
}

eCharData* eServerCharData::getImpl(const int id) {
    if(id < 0 || id >= mChars.size()) {
        eRuntimeThrow("Index out of range.");
    }
    return &mChars.get(id);
}
