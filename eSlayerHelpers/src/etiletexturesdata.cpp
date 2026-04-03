#include "eSlayerHelpers/etiletexturesdata.h"

#include "eSlayerHelpers/efileloaderbase.h"

eTileTexturesData::eTileTexturesData(const std::string& dirName) :
    mDirName(dirName) {}

void eTileTexturesData::loadImpl() {
    if(mLoaded) return;
    mLoaded = true;

    const auto dir = "Textures";

    const auto path = mDirName + "/" + mDirName + ".json";
    const auto jdata = eFileLoaderBase::parse(dir, path);
    const auto names = jdata.get<std::vector<std::string>>();

    for(const auto& name : names) {
        const eTileTextureData texs{mDirName, name};
        mTexs.add(name, texs);
    }
}

int eTileTexturesData::idImpl(const std::string& name) {
    return mTexs.id(name);
}

eTileTextureData& eTileTexturesData::getImpl(const std::string& name) {
    const int id = mTexs.id(name);
    return getImpl(id);
}

eTileTextureData& eTileTexturesData::getImpl(const int id) {
    return mTexs.get(id);
}

void eTileTexturesData::forEachImpl(const eIter& iter) const {
    for(const auto& it : mTexs) {
        iter(it);
    }
}
