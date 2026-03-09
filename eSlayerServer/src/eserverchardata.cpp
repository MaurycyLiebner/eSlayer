#include "eserverchardata.h"

#include "eserverfileloader.h"

eServerCharData eServerCharData::sInstance;

eServerCharData::eServerCharData() {}

eCharData *eServerCharData::get(const std::string& name) {
    return sInstance.getImpl(name);
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

    for(const auto& name : chars) {
        auto& texs = mChars[name];
        texs.setName(name);
        auto jdata = eServerFileLoader::parse(dir, "chars/" + name + "/" + name + ".json");
        texs.load(jdata);
    }
}

eCharData* eServerCharData::getImpl(const std::string &name) {
    const auto it = mChars.find(name);
    if(it == mChars.end()) return nullptr;
    return &it->second;
}
