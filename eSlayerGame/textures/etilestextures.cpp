#include "etilestextures.h"

#include "../efileloader.h"

eTilesTextures::eTilesTextures(const std::string& dirName) :
    mDirName(dirName) {}

void eTilesTextures::loadImpl() {
    if(mLoaded) return;
    mLoaded = true;

    const auto dir = "Textures";

    const auto path = mDirName + "/" + mDirName + ".json";
    const auto jdata = eFileLoader::parse(dir, path);
    const auto names = jdata.get<std::vector<std::string>>();

    for(const auto& name : names) {
        auto& texs = mTexs[name];
        texs.initialize(mDirName, name);
    }
}

eTileTextures* eTilesTextures::getImpl(const std::string& name) {
    const auto it = mTexs.find(name);
    if(it == mTexs.end()) return nullptr;
    return &it->second;
}
