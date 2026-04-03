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
        eTileTextures texs;
        texs.initialize(mDirName, name);
        mTexs.add(name, texs);
    }
}

int eTilesTextures::idImpl(const std::string& name) {
    return mTexs.id(name);
}

eTileTextures& eTilesTextures::getImpl(const std::string& name) {
    const int id = mTexs.id(name);
    return getImpl(id);
}

eTileTextures& eTilesTextures::getImpl(const int id) {
    return mTexs.get(id);
}
