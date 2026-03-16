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
            auto jdata = eServerFileLoader::parse(dir, "chars/" + name + "/" + name + ".json");
            texs.load(jdata);
        } catch(const std::exception& e) {
            eExceptions::showDialog(e);
            mCharIdMap.erase(name);
        }
    }
}

eCharData* eServerCharData::getImpl(const std::string &name) {
    const auto it = mCharIdMap.find(name);
    if(it == mCharIdMap.end()) return nullptr;
    return getImpl(it->second);
}

eCharData* eServerCharData::getImpl(const int id) {
    if(id < 0 || id >= mChars.size()) {
        eRuntimeThrow("Index out of range.");
    }
    return &mChars[id];
}
