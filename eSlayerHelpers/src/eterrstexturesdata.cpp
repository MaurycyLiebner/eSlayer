#include "eSlayerHelpers/eterrstexturesdata.h"

eTerrsTexturesData eTerrsTexturesData::sInstance;

eTerrsTexturesData::eTerrsTexturesData() :
    eTileTexturesData("terrain") {}

int eTerrsTexturesData::id(const std::string& name) {
    return sInstance.idImpl(name);
}

eTileTextureData& eTerrsTexturesData::get(const std::string& name) {
    return sInstance.getImpl(name);
}

eTileTextureData& eTerrsTexturesData::get(const int id) {
    return sInstance.getImpl(id);
}

void eTerrsTexturesData::load() {
    return sInstance.loadImpl();
}

void eTerrsTexturesData::forEach(const eIter& iter) {
    sInstance.forEachImpl(iter);
}
