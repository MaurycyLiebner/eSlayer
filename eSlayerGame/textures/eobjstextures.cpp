#include "eobjstextures.h"

eObjsTextures eObjsTextures::sInstance;

eObjsTextures::eObjsTextures() :
    eTilesTextures("objects") {}

eTileTextures* eObjsTextures::get(const std::string& name) {
    return sInstance.getImpl(name);
}

void eObjsTextures::load() {
    return sInstance.loadImpl();
}
