#include "eterrstextures.h"

eTerrsTextures eTerrsTextures::sInstance;

eTerrsTextures::eTerrsTextures() :
    eTilesTextures("terrain") {}

eTileTextures* eTerrsTextures::get(const std::string& name) {
    return sInstance.getImpl(name);
}

void eTerrsTextures::load() {
    return sInstance.loadImpl();
}
